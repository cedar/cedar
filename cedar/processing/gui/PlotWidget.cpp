/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        PlotWidget.cpp

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.rub.de
    Date:        2013-07-31

    Description:

    Credits:

======================================================================================================================*/
#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/configuration.h"
#include "PlotWidget.h"
#include "cedar/auxiliaries/gui/DataPlotter.h"
#include "cedar/processing/exceptions.h"


// SYSTEM INCLUDES
// none yet

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PlotWidget::PlotWidget
(
    cedar::proc::StepPtr step,
    const cedar::proc::ElementDeclaration::DataList& data
)
:
mDataList(data),
mStep(step),
mCurrentLabeledPlot(NULL),
mGridSpacing(2),
mColumns(2),
mpLayout(new QGridLayout())
{  
  if (mDataList.empty())
  {
    return;
  }

  // initialize layout
  this->setContentsMargins(0, 0, 0, 0);
  mpLayout->setContentsMargins(mGridSpacing, mGridSpacing, mGridSpacing, mGridSpacing);
  mpLayout->setSpacing(mGridSpacing);
  this->setLayout(mpLayout);

  fillGridWithPlots();

  // make all columns have the same stretch factor
  for(int column = 0; column < mpLayout->columnCount(); ++column)
  {
    mpLayout->setColumnStretch(static_cast<size_t>(column), 1);
  }
}

cedar::proc::gui::PlotWidget::~PlotWidget()
{
  for(auto connection : mSignalConnections)
  {
    connection.disconnect();
  }
  mSignalConnections.clear();
  mDataList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// read out slot data and open/add-to plot
bool cedar::proc::gui::PlotWidget::processSlot
(
  cedar::aux::ConstDataPtr pData,
  cedar::proc::PlotDataPtr dataItem,
  const std::string& title
)
{
  bool is_multiplot;

  // get the plot_class
  std::string plot_declaration = dataItem->getParameter<cedar::aux::StringParameter>("plotDeclaration")->getValue();
  auto declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find(pData)->getData();
  auto decl = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(pData);
  
  for(auto declaration : declarations)
  {
    if(declaration->getClassName() == plot_declaration)
    {
      decl = declaration;
      break;
    }
  }
  if
    // if no plotter exists or the declaration demands a different plotter or we cannot append
    // the data to the current plotter, we have to create a new plotter and have it plot the data
   (
      mCurrentLabeledPlot.mpPlotter == NULL
      || 
        (
          plot_declaration != ""
          && mCurrentLabeledPlot.mpPlotDeclaration->getClassName() != plot_declaration
        )
      ||
        !(
          dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(mCurrentLabeledPlot.mpPlotter)
          && static_cast<cedar::aux::gui::MultiPlotInterface*>(mCurrentLabeledPlot.mpPlotter)->canAppend(pData)
        )
    )
  {
    is_multiplot = !createAndAddPlotToGrid(decl, pData, title);
  }
  else
  {
    is_multiplot = tryAppendDataToPlot(pData, title);
  }

  return is_multiplot;
}

// iterates of the dataslots given in mDataList, creates plots for the data and adds them to the grid
void cedar::proc::gui::PlotWidget::fillGridWithPlots()
{
  bool is_multiplot = false;
  std::string title;
  cedar::proc::ElementDeclaration::DataList invalid_data;

  auto process_if_set = [&]
    (
      cedar::aux::ConstDataPtr data,
      cedar::proc::PlotDataPtr data_item
    )
    {
      if (data)
      // skip slots that aren't set
      {
        is_multiplot = processSlot(data, data_item, title);
      }
      else
      {
        // mark for removal
        invalid_data.push_back(data_item);
      }
    };

  // iterate over all data slots
  for(auto data_item : mDataList)
  {
    try
    {
      cedar::proc::DataSlotPtr p_slot = mStep->getSlot
      (
        data_item->getParameter<cedar::aux::EnumParameter>("id")->getValue(),
        data_item->getParameter<cedar::aux::StringParameter>("name")->getValue()
      );
      title = p_slot->getText();

      // we need to treat external data differently
      if(auto p_input_slot = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(p_slot))
      {
        bool multiplot_was_true = false;
        // could check if it's a collection with p_input_slot->isCollection();
        // but if it's not this loop should have just one iteration ...
        for(size_t i = 0; i < p_input_slot->getDataCount(); ++i)
        {
          process_if_set(p_input_slot->getData(i), data_item);
          // if multiplot was once true it should be true at the end too
          multiplot_was_true = multiplot_was_true || is_multiplot;
          is_multiplot = false;
        }
        is_multiplot = multiplot_was_true;

        // handle disconnection of slot-input
        this->mSignalConnections.push_back
        (
          p_input_slot->connectToExternalDataRemoved
          (
            boost::bind
            (
              &cedar::proc::gui::PlotWidget::removePlotOfExternalData,
              this,
              _1,
              is_multiplot
            )
          )
        );

        // react to addition of input to a slot
        this->mSignalConnections.push_back
        (
          p_input_slot->connectToExternalDataAdded
          (
            boost::bind
            (
              &cedar::proc::gui::PlotWidget::addPlotOfExternalData,
              this,
              _1,
              p_input_slot,
              data_item
            )
          )
        );
      }
      else // internal data, no need to worry about changes in the slot. plot_widget lives as long as internal data lives
      {
        process_if_set(p_slot->getData(), data_item);
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      if (!data_item->getParameter<cedar::aux::BoolParameter>("ignoreIfMissing"))
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Could not plot data. Exception: " + e.exceptionInfo(),
          "cedar::proc::gui::PlotWidget::PlotWidget()"
        );
      }
    }
    catch (const cedar::aux::InvalidNameException& e)
    {
      if (!data_item->getParameter<cedar::aux::BoolParameter>("ignoreIfMissing"))
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Could not plot data. Exception: " + e.exceptionInfo(),
          "cedar::proc::gui::PlotWidget::PlotWidget()"
        );
      }
    }
  }

  // remove empty slots from the datalist
  for(auto& invalid_data_item : invalid_data)
  {
    mDataList.erase(std::remove(mDataList.begin(), mDataList.end(), invalid_data_item), mDataList.end());
  }
  // if there is only one plot and it is a multiplot, we need no label
  if (is_multiplot && mPlotGridMap.size() == 1)
  {
    delete mCurrentLabeledPlot.mpLabel;
    mCurrentLabeledPlot.mpLabel = NULL;
    mPlotGridMap.begin()->second.mpLabel = NULL;
  }
}

// returns the next free slot in the grid layout and marks it as taken IF it was previously freed
// it doesn't mark it as taken if it is a 'brand new' slot
std::tuple<int, int> cedar::proc::gui::PlotWidget::usingNextFreeGridSlot()
{
  std::tuple<int, int> free_grid_slot;
  if(this->mFreeGridSlots.size() > 0)
  {
    free_grid_slot = this->mFreeGridSlots.front();
    this->mFreeGridSlots.pop_front();
  }
  else
  {
    // each plot consists of a separate label and widget below the label,
    // i.e. for every plot we add 2 items to the grid in the same column
    // therefore we calculate the next free 'cell' like this
    int row = ((this->mpLayout->count() / 2) / this->mColumns) * 2;
    // notice that we're dealing with int and thus the / 2 and * 2 don't negate each other!
    int col = ((this->mpLayout->count() / 2) % this->mColumns);
    free_grid_slot = std::make_tuple(row, col);
  }

  return free_grid_slot;
}

bool cedar::proc::gui::PlotWidget::createAndAddPlotToGrid
(
  cedar::aux::gui::ConstPlotDeclarationPtr decl,
  cedar::aux::ConstDataPtr pData,
  const std::string& title
)
{
  bool success = true;
  int row, column;
  // get the next free slot in the grid layout
  std::tie(row, column) = this->usingNextFreeGridSlot();
  mCurrentLabeledPlot = LabeledPlot(new QLabel(QString::fromStdString(title)), decl);
  mpLayout->addWidget(mCurrentLabeledPlot.mpLabel, row, column);
  mpLayout->setRowStretch(row, 0);
  try
  {
    mCurrentLabeledPlot.mpPlotter->plot(pData, title);
    mpLayout->addWidget(mCurrentLabeledPlot.mpPlotter, row + 1, column);
    mpLayout->setRowStretch(row + 1, 1);
    // store the LabeledPlot in the PlotGridMap
    mPlotGridMap.insert(PlotGridMapItem(pData, mCurrentLabeledPlot));
  }
  catch (cedar::aux::UnknownTypeException& exc)
  {
    // clean up allocated data
    delete mCurrentLabeledPlot.mpPlotter;
    mCurrentLabeledPlot.mpPlotter = NULL;
    delete mCurrentLabeledPlot.mpLabel;
    mCurrentLabeledPlot.mpLabel = NULL;
    success = false;
  }

  return success;
}

bool cedar::proc::gui::PlotWidget::tryAppendDataToPlot
(
  cedar::aux::ConstDataPtr pData,
  const std::string& title
)
{
  try
  {
    cedar::aux::asserted_cast<cedar::aux::gui::MultiPlotInterface*>(mCurrentLabeledPlot.mpPlotter)->append(pData, title);
    mCurrentLabeledPlot.mpLabel->setText("");
    // store the labeled plot again, with a different key (there now are at least 2 entries for this plot)
    mPlotGridMap.insert(PlotGridMapItem(pData, mCurrentLabeledPlot));
    return true;
  }
  catch (cedar::aux::UnknownTypeException& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not append data. Exception: " + e.exceptionInfo(),
      "cedar::proc::gui::PlotWidget::tryAppendData"
    );
    return false;
  }
}

int cedar::proc::gui::PlotWidget::getColumnCount() const
{
  return mpLayout->columnCount();
}

int cedar::proc::gui::PlotWidget::getRowCount() const
{
  //we half the row count since we doubled it during construction (for labels)
  return mpLayout->rowCount() / 2;
}

void cedar::proc::gui::PlotWidget::addPlotOfExternalData
(
  cedar::aux::ConstDataPtr pData,
  cedar::proc::ExternalDataPtr slot,
  cedar::proc::PlotDataPtr dataItem
)
{
  if(slot->hasData(pData))
  {
    processSlot(pData, dataItem, slot->getText());
  }
}

void cedar::proc::gui::PlotWidget::removePlotOfExternalData
(
  cedar::aux::ConstDataPtr pData,
  bool isMultiplot
)
{
  auto plot_grid_map_item = mPlotGridMap.find(pData);

  if(isMultiplot)
  {
    try
    {
      // this doesn't exist yet:
      // cedar::aux::asserted_cast<cedar::aux::gui::MultiPlotInterface*>
      //    (
      //      cedar::aux::asserted_cast<cedar::proc::gui::PlotWidget::LabeledPlot>(plot).mpPlotter
      //    )->remove(pData);
    }
    // THIS CATCH IS ONLY A PLACEHOLDER, don't know which exception might be thrown by not yet implemented method 'remove'
    catch (cedar::aux::UnknownTypeException& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not remove data from Multiplot. Exception: " + e.exceptionInfo(),
        "cedar::proc::gui::PlotWidget::removePlotOfExternalData"
      );
    }
  }
  else
  {
    auto& labeled_plot = plot_grid_map_item->second;

    // remove the widget from the grid, and mark that slot as free
    auto index = this->mpLayout->indexOf(labeled_plot.mpLabel);
    int row, col, r_span, c_span;
    // get row and column of the label
    this->mpLayout->getItemPosition(index, &row, &col, &r_span, &c_span);

    // no idea if takeAt changes the indices of other items, to be safe take the plot first
    labeled_plot.mpPlotter = this->mpLayout->takeAt(index + 1);
    labeled_plot.mpLabel = this->mpLayout->takeAt(index);

    // store the slot at the end of the list of free slots
    mFreeGridSlots.push_back(std::make_tuple(row, col));

    // free the old plot and label
    delete labeled_plot.mpPlotter;
    delete labeled_plot.mpLabel;

    // remove the entry that maps the data to the now removed plot
    this->mPlotGridMap.erase(plot_grid_map_item);
  }
}

// save this plot_widget in configuration node
void cedar::proc::gui::PlotWidget::writeConfiguration(cedar::aux::ConfigurationNode& root)
{
  root.put("step", this->mStep->getName());
  root.put("position_x", this->parentWidget()->x());
  root.put("position_y", this->parentWidget()->y());
  root.put("width", this->parentWidget()->width());
  root.put("height", this->parentWidget()->height());
  root.put_child("data_list", serialize(this->mDataList));
}

// serialize the datalist for storing it in a configuration node
cedar::aux::ConfigurationNode cedar::proc::gui::PlotWidget::serialize(const cedar::proc::ElementDeclaration::DataList& dataList) const
{
  cedar::aux::ConfigurationNode serialized_data;
  
  for (auto data_item: dataList)
  {
    cedar::aux::ConfigurationNode value_node;
    data_item->writeConfiguration(value_node);
    serialized_data.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
  }
  
  return serialized_data;
}

// restore plotwidget from a configuration node and add it to the respective step
void cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(const cedar::aux::ConfigurationNode& node, cedar::proc::gui::StepItem* pStepItem)
{
  int width = node.get<int>("width");
  int height = node.get<int>("height");
  int x = node.get<int>("position_x");
  int y = node.get<int>("position_y");
  
  auto serialized_data_list = node.get_child("data_list");
  cedar::proc::ElementDeclaration::DataList data_list;
  
  for(auto data_item : serialized_data_list)
  {
    auto p_plot_data = cedar::proc::PlotDataPtr(new cedar::proc::PlotData());
    p_plot_data->readConfiguration(data_item.second);
    data_list.push_back(p_plot_data);
  }
  
  auto p_plot_widget = new cedar::proc::gui::PlotWidget(pStepItem->getStep(), data_list);
  pStepItem->addPlotWidget(p_plot_widget, x, y, width, height);
}