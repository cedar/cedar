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
mData(data),
mStep(step),
mLabeledPlot(NULL),
mGridSpacing(2),
mColumns(2),
mpLayout(new QGridLayout())
{  
  if (mData.empty())
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
  for (int column = 0; column < mpLayout->columnCount(); ++column)
  {
    mpLayout->setColumnStretch(column, 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PlotWidget::fillGridWithPlots()
{
  // iterate over all data slots
  int count = 0;
  bool is_multiplot = false;
  std::vector<cedar::proc::ElementDeclaration::DataList::iterator> invalid_data;

  // create lambda to either call in loop or just once, depending on slot_type

  auto processSlot = [&](cedar::proc::DataSlotPtr slot, cedar::proc::PlotDataPtr data_item)
  {
    cedar::aux::DataPtr p_data = slot->getData();
    const std::string& title = slot->getText();

    if (p_data)
    // skip slots that aren't set
    {
      // get the plot_class
      std::string plot_declaration = data_item->getParameter<cedar::aux::StringParameter>("plotDeclaration")->getValue();
      auto declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find(p_data)->getData();
      auto decl = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(p_data);
      
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
          mLabeledPlot.mpPlotter == NULL
          || 
            (
              plot_declaration != ""
              && mLabeledPlot.mpPlotDeclaration->getClassName() != plot_declaration
            )
          ||
            !(
              dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(mLabeledPlot.mpPlotter)
              && static_cast<cedar::aux::gui::MultiPlotInterface*>(mLabeledPlot.mpPlotter)->canAppend(p_data)
            )
        )
      {
        //count += createAndAddPlotToGrid(decl, p_data, title, row, column) ? 1 : 0;
        count += createAndAddPlotToGrid(decl, p_data, title, 2 * (count / mColumns), count % mColumns) ? 1 : 0;
      }
      else
      {
        is_multiplot = tryAppendDataToPlot(p_data, title);
      }
    }
    else
    {
      // mark for removal
      invalid_data.push_back(it);
    }
  }

  for (auto data_item : mData)
  {
    try
    {
      cedar::proc::DataSlotPtr slot = mStep->getSlot(data_item->getParameter<cedar::aux::EnumParameter>("id")->getValue(), data_item->getParameter<cedar::aux::StringParameter>("name")->getValue());

      // input slots can be collections so check if that's the case
      if(auto input_slot = boost::dynamic_pointer_cast<cedar::proc::ExternalDataPtr>(slot) && input_slot->isCollection())
      {
        for(auto i : input_slot->getDataCount())
        {
          processSlot(input_slot->getData(i), data_item);
        }
      }
      else
      {
        processSlot(slot, data_item);
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

  for(auto it : invalid_data)
  {
    mData.erase(it);
  }
  // if there is only one plot and it is a multiplot, we need no label
  if (count == 1 && is_multiplot)
  {
    delete mLabeledPlot.mpLabel;
    mLabeledPlot.mpLabel = NULL;
  }
}

bool cedar::proc::gui::PlotWidget::createAndAddPlotToGrid
(
  cedar::aux::gui::ConstPlotDeclarationPtr decl,
  cedar::aux::DataPtr pData,
  const std::string& title,
  int row,
  int column
)
{
  bool success = true;

  mLabeledPlot = LabeledPlot(new QLabel(QString::fromStdString(title)), decl);
  mpLayout->addWidget(mLabeledPlot.mpLabel, row, column);
  mpLayout->setRowStretch(row, 0);
  try
  {
    mLabeledPlot.mpPlotter->plot(pData, title);
    mpLayout->addWidget(mLabeledPlot.mpPlotter, row + 1, column);
    mpLayout->setRowStretch(row + 1, 1);
  }
  catch (cedar::aux::UnknownTypeException& exc)
  {
    // clean up allocated data
    delete mLabeledPlot.mpPlotter;
    mLabeledPlot.mpPlotter = NULL;
    delete mLabeledPlot.mpLabel;
    mLabeledPlot.mpLabel = NULL;
    success = false;
  }

  return success;
}

bool cedar::proc::gui::PlotWidget::tryAppendDataToPlot
(
  cedar::aux::DataPtr pData,
  const std::string& title
)
{
  try
  {
    cedar::aux::asserted_cast<cedar::aux::gui::MultiPlotInterface*>(mLabeledPlot.mpPlotter)->append(pData, title);
    mLabeledPlot.mpLabel->setText("");
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

int cedar::proc::gui::PlotWidget::getColumnCount()
{
  return mpLayout->columnCount();
}

int cedar::proc::gui::PlotWidget::getRowCount()
{
  //we half the row count since we doubled it during construction (for labels)
  return mpLayout->rowCount() / 2;
}

void cedar::proc::gui::PlotWidget::writeConfiguration(cedar::aux::ConfigurationNode& root)
{
  root.put("step", this->mStep->getName());
  root.put("position_x", this->parentWidget()->x());
  root.put("position_y", this->parentWidget()->y());
  root.put("width", this->parentWidget()->width());
  root.put("height", this->parentWidget()->height());
  root.put_child("data_list", serialize(this->mData));
}

cedar::aux::ConfigurationNode cedar::proc::gui::PlotWidget::serialize(const cedar::proc::ElementDeclaration::DataList& dataList) const
{
  cedar::aux::ConfigurationNode serialized_data;
  
  for (auto it = dataList.begin(); it != dataList.end(); ++it)
  {
    auto deit = *it;
    cedar::aux::ConfigurationNode value_node;
    deit->writeConfiguration(value_node);
    serialized_data.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
  }
  
  return serialized_data;
}

void cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(const cedar::aux::ConfigurationNode& node, cedar::proc::gui::StepItem* pStepItem)
{
  int width = node.get<int>("width");
  int height = node.get<int>("height");
  int x = node.get<int>("position_x");
  int y = node.get<int>("position_y");
  
  auto serialized_data_list = node.get_child("data_list");
  cedar::proc::ElementDeclaration::DataList data_list;
  
  for(auto it = serialized_data_list.begin(); it != serialized_data_list.end(); ++it)
  {
    auto p_plot_data = cedar::proc::PlotDataPtr(new cedar::proc::PlotData());
    p_plot_data->readConfiguration(it->second);
    data_list.push_back(p_plot_data);
  }
  
  auto p_plot_widget = new cedar::proc::gui::PlotWidget(pStepItem->getStep(), data_list);
  pStepItem->addPlotWidget(p_plot_widget, x, y, width, height);
}