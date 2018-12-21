/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/Group.h"
#include "cedar/processing/gui/Settings.h"


// SYSTEM INCLUDES
#include <boost/make_shared.hpp>
#ifdef CEDAR_USE_QT5
    #include <QSizeGrip>
#else
    #include <QtGui/QSizeGrip>
#endif // CEDAR_USE_QT5

//----------------------------------------------------------------------------------------------------------------------
// private nested structs and classes
//----------------------------------------------------------------------------------------------------------------------

//!@cond SKIPPED_DOCUMENTATION
cedar::proc::gui::PlotWidgetPrivate::LabeledPlot::LabeledPlot
(
  const QString& pLabel,
  cedar::aux::ConstDataPtr pData,
  cedar::proc::PlotDataPtr pPlotData,
  const std::string& declarationToUse
)
:
//mpPlotDeclaration(pPlotDecl),
mpPlotSelector(nullptr),
mpPlotter(nullptr),
mIsMultiPlot(false),
mpData(pData),
mpPlotData(pPlotData),
mTitle(pLabel.toStdString()),
mpLabel(new QLabel(pLabel)),
mGridMinimumHeight(150),
mGridMinimumWidth(200)
{
  this->mpTitleLayout = new QHBoxLayout();
  this->mpTitleLayout->setContentsMargins(0, 0, 0, 0);

  this->mpPlotSelector = new QPushButton();
  this->mpPlotSelector->setFixedSize(20, 20);
  auto menu = new QMenu();
  this->fillPlotOptions(menu);
  this->mpPlotSelector->setMenu(menu);

  this->mpPlotContainer = new QWidget();
  auto layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  this->mpPlotContainer->setLayout(layout);

  auto widthHeight = cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize();

//  std::cout<<"Created a PlotWidget and received the current width: "<< widthHeight.x() << " and height: " << widthHeight.y() << std::endl;
  //This is the sample resolution of a screen in the roboticslab
  //Todo: Minimum Width and Height should be updated once the screen size changes, but this problem is not urgent enough!
  float heuristicX = 1920;
  float heuristicY = 1030;
  float widthFactor = (float) widthHeight.x() / heuristicX;
  float heightFactor = (float) widthHeight.y() / heuristicY;

  this->mpPlotContainer->setMinimumWidth((int)mGridMinimumWidth*widthFactor);
  this->mpPlotContainer->setMinimumHeight((int)mGridMinimumHeight*heightFactor);

  this->mpTitleLayout->addWidget(this->mpPlotSelector);
  this->mpTitleLayout->addWidget(this->mpLabel);

  this->openPlotFromDeclaration(declarationToUse);
};

void cedar::proc::gui::PlotWidgetPrivate::LabeledPlot::openPlotFromDeclaration(const std::string& declarationToFind)
{
  // first, check if there are any declarations for the data at all
  cedar::aux::gui::ConstPlotDeclarationPtr decl;
  try
  {
    mpPlotDeclaration = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(mpData);
  }
  catch (cedar::aux::NotFoundException&)
  {
    // ok -- below, we display info to the user
  }
  catch (cedar::aux::UnknownTypeException)
  {
    // ok -- below, we display info to the user
  }

  if (!declarationToFind.empty())
  {
    // then, try to find one that matches the specified one
    try
    {
      auto declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find(mpData)->getData();
      for (auto declaration : declarations)
      {
        if (declaration->getClassName() == declarationToFind)
        {
          mpPlotDeclaration = declaration;
          break;
        }
      }
    }
    catch (cedar::aux::UnknownTypeException)
    {
    }
  }

  if (this->mpPlotter)
  {
    // remember the old data that was plotted
    if (auto multi = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpPlotter))
    {
      const auto& map = multi->getDataTitleMap();
      mMultiPlotData.insert(map.begin(), map.end());
    }

    this->mpPlotContainer->layout()->removeWidget(this->mpPlotter);
    delete this->mpPlotter;
  }

  // clear all remaining widgets from the plot container
  while (this->mpPlotContainer->layout()->count() > 0)
  {
    auto item = this->mpPlotContainer->layout()->itemAt(0);
    auto widget = item->widget();
    this->mpPlotContainer->layout()->removeItem(item);
    delete widget;
  }

  if (mpPlotDeclaration)
  {
    try
    {
      mpPlotter = mpPlotDeclaration->createPlot();
      this->mpPlotter->plot(mpData, mTitle);
      this->mpPlotContainer->layout()->addWidget(mpPlotter);
      this->mpPlotData->setPlotDeclaration(mpPlotDeclaration->getClassName());
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      auto label = new QLabel("Could not open plot. Hover over this text for details.");
      label->setWordWrap(true);
      label->setToolTip(QString::fromStdString(e.exceptionInfo()));
      this->mpPlotContainer->layout()->addWidget(label);
    }

    // append the remaining data
    if (!mMultiPlotData.empty())
    {
      if (auto multi = dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(this->mpPlotter))
      {
        for (auto data_name_pair : mMultiPlotData)
        {
          const auto& data = data_name_pair.first;
          const auto& name = data_name_pair.second;

          if (multi->canAppend(data))
          {
            multi->append(data, name);
          }
        }
      }
    }
  }
  else // No plot declaration found
  {
    auto label = new QLabel("Could not open plot: no known plot for type.");
    label->setWordWrap(true);
    this->mpPlotContainer->layout()->addWidget(label);
  }
}

void cedar::proc::gui::PlotWidgetPrivate::LabeledPlot::openDefaultPlot()
{
  // empty = default plot
  this->openPlotFromDeclaration(std::string());
}

void cedar::proc::gui::PlotWidgetPrivate::LabeledPlot::openSpecificPlot()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(action);
  this->openPlotFromDeclaration(action->text().toStdString());
}

void cedar::proc::gui::PlotWidgetPrivate::LabeledPlot::fillPlotOptions(QMenu* menu)
{
  this->mpPlotSelector->setEnabled(true);
  auto default_action = menu->addAction("default");
  menu->addSeparator();
  QObject::connect(default_action, SIGNAL(triggered()), this, SLOT(openDefaultPlot()));

  try
  {
    auto declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find(mpData)->getData();
    for (auto declaration : declarations)
    {
      auto action = menu->addAction(QString::fromStdString(declaration->getClassName()));
      QObject::connect(action, SIGNAL(triggered()), this, SLOT(openSpecificPlot()));
    }
  }
  catch (cedar::aux::UnknownTypeException)
  {
    default_action->setEnabled(false);
    auto no_action = menu->addAction("no plots found");
    no_action->setEnabled(false);
    this->mpPlotSelector->setEnabled(false);
  }
}
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PlotWidget::PlotWidget
(
    cedar::proc::ConnectablePtr connectable,
    const cedar::proc::ElementDeclaration::DataList& data
)
:
mConnectable(connectable),
mGridSpacing(2),
mColumns(2),
mpLayout(new QGridLayout()),
mWidgetLabel("")
{  
  // make a copy of the data to be plotted
  this->mDataList.clear();
  for (const auto& plot_data : data)
  {
    this->mDataList.push_back(boost::make_shared<cedar::proc::PlotData>(*plot_data));
  }

  if (mDataList.empty())
  {
    return;
  }

  // initialize layout
  this->setContentsMargins(0, 0, 0, 0);
  this->mpLayout->setContentsMargins(mGridSpacing, mGridSpacing, mGridSpacing, mGridSpacing);
  this->mpLayout->setSpacing(mGridSpacing);



  this->setLayout(mpLayout);

  fillGridWithPlots();

//  std::cout<<"Constructor: Number of Rows: " << this->mpLayout->rowCount() << "! Number of Columns: " << this->mpLayout->columnCount() << std::endl;

  this->mpLayout->addWidget(new QSizeGrip(this),this->mpLayout->rowCount()-1,this->mpLayout->columnCount()-1, Qt::AlignBottom | Qt::AlignRight);

  // make all columns have the same stretch factor
  //!@todo Shouldn't this happen every time the plot is filled, i.e., within fillGridWithPlots?
  for (int column = 0; column < mpLayout->columnCount(); ++column)
  {
    this->mpLayout->setColumnStretch(static_cast<size_t>(column), 1);
  }



}

cedar::proc::gui::PlotWidget::~PlotWidget()
{
  for(auto connection : this->mSignalConnections)
  {
    connection.disconnect();
  }
  this->mSignalConnections.clear();
  this->mDataList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// read out slot data and open/add-to plot
void cedar::proc::gui::PlotWidget::processSlot
(
  cedar::aux::ConstDataPtr pData,
  cedar::proc::PlotDataPtr dataItem,
  const std::string& title
)
{
  // get the plot_class
  std::string plot_declaration = dataItem->getParameter<cedar::aux::StringParameter>("plotDeclaration")->getValue();

  auto plot_declaration_does_match = [&](LabeledPlotPtr p_labeled_plot)
  {
    // no plot_declaration matches any declaration.
    return (plot_declaration == "" || p_labeled_plot->mpPlotDeclaration->getClassName() == plot_declaration);
  };
  auto can_append_p_data_to = [&](LabeledPlotPtr p_labeled_plot)
  {
    return (dynamic_cast<cedar::aux::gui::MultiPlotInterface*>(p_labeled_plot->mpPlotter)
            && static_cast<cedar::aux::gui::MultiPlotInterface*>(p_labeled_plot->mpPlotter)->canAppend(pData));
  };

  LabeledPlotPtr p_current_labeled_plot;
  bool do_append = false;
  for (auto plot_grid_map_item : this->mPlotGridMap)
  {
    auto p_labeled_plot = plot_grid_map_item.second;
    if (plot_declaration_does_match(p_labeled_plot) && can_append_p_data_to(p_labeled_plot))
    {
      p_current_labeled_plot = p_labeled_plot;
      do_append = true;
      break;
    }
  }

  if (do_append)
    // if we did not find a plotter that matches the declaration AND can append data we create a new plotter
  {
    tryAppendDataToPlot(pData, title, p_current_labeled_plot);
  }
  else
  {
    createAndAddPlotToGrid(plot_declaration, pData, dataItem, title);
  }
}

// iterates of the dataslots given in mDataList, creates plots for the data and adds them to the grid
void cedar::proc::gui::PlotWidget::fillGridWithPlots()
{
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
      processSlot(data, data_item, title);
    }
    else
    {
      // mark for removal
      invalid_data.push_back(data_item);
    }
  };

  // iterate over all data slots
  for (auto data_item : mDataList)
  {
    try
    {
      cedar::proc::DataSlotPtr p_slot = mConnectable->getSlot
      (
        data_item->getParameter<cedar::aux::EnumParameter>("id")->getValue(),
        data_item->getParameter<cedar::aux::StringParameter>("name")->getValue()
      );
      title = p_slot->getText();

      // we need to treat external data differently
      if (auto p_input_slot = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(p_slot))
      {
        // could check if it's a collection with p_input_slot->isCollection();
        // but if it's not this loop should have just one iteration ...
        for (size_t i = 0; i < p_input_slot->getDataCount(); ++i)
        {
          process_if_set(p_input_slot->getData(i), data_item);
        }

        // handle disconnection of slot-input
        this->mSignalConnections.push_back
        (
          p_input_slot->connectToDataRemovedSignal
          (
            boost::bind
            (
              &cedar::proc::gui::PlotWidget::removePlotOfExternalData,
              this,
              _1
            )
          )
        );

        // react to addition of input to a slot
        this->mSignalConnections.push_back
        (
          p_input_slot->connectToDataSetSignal
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
  for (auto& invalid_data_item : invalid_data)
  {
    mDataList.erase(std::remove(mDataList.begin(), mDataList.end(), invalid_data_item), mDataList.end());
  }

  // if there is only one plot and it is a multiplot, we need no label
  if (this->mPlotGridMap.empty())
  {
    // nothing to do, return
    return;
  }

  auto p_current_labeled_plot = mPlotGridMap.begin()->second;
  if (mPlotGridMap.size() == 1 && p_current_labeled_plot->mIsMultiPlot)
  {
    delete p_current_labeled_plot->mpLabel;
    p_current_labeled_plot->mpLabel = nullptr;
  }
//  std::cout<<"FillGridWithPlots!: Number of Rows: " << this->getRowCount() << "! Number of Columns: " << this->getColumnCount() << std::endl;
}

// returns the next free slot in the grid layout and marks it as taken IF it was previously freed
// it doesn't mark it as taken if it is a 'brand new' slot
std::tuple<int, int> cedar::proc::gui::PlotWidget::usingNextFreeGridSlot()
{
  std::tuple<int, int> free_grid_slot;
  if (this->mFreeGridSlots.size() > 0)
  {
    this->mFreeGridSlots.sort([](std::tuple<int, int> a, std::tuple<int, int> b)
      {
        // true if a goes before b, false otherwise
        if(std::get<0>(a) > std::get<0>(b))
        {
          return false;
        }
        if(std::get<0>(a) == std::get<0>(b) && std::get<1>(a) > std::get<1>(b))
        {
          return false;
        }

        return true;
      }
    );
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

void cedar::proc::gui::PlotWidget::createAndAddPlotToGrid
(
  const std::string& decl,
  cedar::aux::ConstDataPtr pData,
  cedar::proc::PlotDataPtr dataItem,
  const std::string& title
)
{
  int row, column;
  // get the next free slot in the grid layout
  std::tie(row, column) = this->usingNextFreeGridSlot();
  LabeledPlotPtr p_current_labeled_plot(new LabeledPlot(QString::fromStdString(title), pData, dataItem, decl));

  mpLayout->addLayout(p_current_labeled_plot->mpTitleLayout, row, column);
  mpLayout->setRowStretch(row, 0);
  try
  {
    mpLayout->addWidget(p_current_labeled_plot->mpPlotContainer, row + 1, column);
    mpLayout->setRowStretch(row + 1, 1);
    // set column stretch in case the number of columns changed
    mpLayout->setColumnStretch(column, 1);
    // store the LabeledPlot in the PlotGridMap
    mPlotGridMap.insert(PlotGridMapItem(pData, p_current_labeled_plot));
  }
  catch (cedar::aux::UnknownTypeException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not plot data. Exception: " + exc.exceptionInfo(),
      "cedar::proc::gui::PlotWidget::createAndAddPlotToGrid"
    );
    // clean up allocated data
    delete p_current_labeled_plot->mpPlotter;
    p_current_labeled_plot->mpPlotter = nullptr;
    delete p_current_labeled_plot->mpTitleLayout;
    p_current_labeled_plot->mpTitleLayout = nullptr;
  }
}

void cedar::proc::gui::PlotWidget::tryAppendDataToPlot
(
  cedar::aux::ConstDataPtr pData,
  const std::string& title,
  LabeledPlotPtr pCurrentLabeledPlot
)
{
  try
  {
    cedar::aux::asserted_cast<cedar::aux::gui::MultiPlotInterface*>(pCurrentLabeledPlot->mpPlotter)->append(pData, title);
    pCurrentLabeledPlot->mpLabel->setText("");
    pCurrentLabeledPlot->mIsMultiPlot = true;
    // store the labeled plot again, with a different key (there now are at least 2 entries for this plot)
    mPlotGridMap.insert(PlotGridMapItem(pData, pCurrentLabeledPlot));
  }
  catch (cedar::aux::UnknownTypeException& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not append data. Exception: " + e.exceptionInfo(),
      "cedar::proc::gui::PlotWidget::tryAppendData"
    );
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

std::tuple<int, int> cedar::proc::gui::PlotWidget::findGridPositionOf(LabeledPlotPtr plot)
{
  for (int r = 0; r < this->mpLayout->rowCount(); ++r)
  {
    for (int c = 0; c < this->mpLayout->columnCount(); ++c)
    {
      auto item = this->mpLayout->itemAtPosition(r, c);
      // check for nullptr before accessing and comparing layout
      if (item != nullptr && item->layout() == plot->mpTitleLayout)
      {
        return std::make_tuple(r, c);
      }
    }
  }

  CEDAR_THROW(cedar::aux::NotFoundException, "Plot not found in layout.");
}

void cedar::proc::gui::PlotWidget::removePlotOfExternalData
(
  cedar::aux::ConstDataPtr pData
)
{
  auto plot_grid_map_item = mPlotGridMap.find(pData);
  // if this data is not displayed, we do not have to remove it
  //!@todo investigate why this happens in the first place!
  if (plot_grid_map_item == mPlotGridMap.end())
  {
    return;
  }

  auto labeled_plot = plot_grid_map_item->second;

  if (labeled_plot->mIsMultiPlot)
  {
    auto multi_plotter = cedar::aux::asserted_cast<cedar::aux::gui::MultiPlotInterface*>(labeled_plot->mpPlotter);
    if(multi_plotter->canDetach(pData))
    {
      multi_plotter->detach(pData);
    }
    else
    { // really this should never happen.
      CEDAR_THROW(cedar::aux::ExceptionBase, "Invalid internal state. Not a multiplot.");
    }
  }
  else
  {
    // get the slot the plot occupies and mark that slot as free
    int row, col;
    QLayoutItem* p_layout_item = nullptr;
    try
    {
      std::tie(row, col) = this->findGridPositionOf(labeled_plot);
      mFreeGridSlots.push_back(std::make_tuple(row, col));
      p_layout_item = this->mpLayout->itemAtPosition(row, col);
    }
    catch (cedar::aux::NotFoundException&)
    {
      // ignored
      cedar::aux::LogSingleton::getInstance()->debugMessage("Could not find plot.", CEDAR_CURRENT_FUNCTION_NAME);
    }

    // now remove the widgets
    if (p_layout_item)
    {
      this->removeFromQGridlayout(p_layout_item);
    }
    remove_qgridlayout_widget(labeled_plot->mpPlotContainer);
    // remove the entry that maps the data to the now removed plot
    this->mPlotGridMap.erase(plot_grid_map_item);
    // when we go out of scope no pointer to this plotter should remain since we removed deleted its members' memory
  }
}

void cedar::proc::gui::PlotWidget::removeFromQGridlayout(QLayoutItem* item)
{
  this->mpLayout->removeItem(item);
  QLayout* sublayout = item->layout();
  for (int i = 0; i < sublayout->count(); ++i)
  {
    if (sublayout->itemAt(i)->widget() != nullptr)
    {
      sublayout->itemAt(i)->widget()->hide();
    }
  }
}

void cedar::proc::gui::PlotWidget::remove_qgridlayout_widget(QWidget* widget)
{
  // removes a qidget from the layout, you're welcome to find a better way
  widget->hide();
  // call to removeWidget deletes the allocated memory
  this->mpLayout->removeWidget(widget);
}

// save this plot_widget in configuration node
void cedar::proc::gui::PlotWidget::writeConfiguration(cedar::aux::ConfigurationNode& root)
{
  std::string step_name = this->mConnectable->getName();
  auto group = this->mConnectable->getGroup();
  CEDAR_ASSERT(group);
  //!@todo shouldn't this use the root group's method for finding the item's path?
  while (group->getGroup()) // group->getName() != "root")
  {
    step_name = group->getName() + "." + step_name;
    group = group->getGroup();
  }
  root.put("step", step_name);
  root.put("position_x", this->parentWidget()->x());
  root.put("position_y", this->parentWidget()->y());
  root.put("width", this->parentWidget()->width());
  root.put("height", this->parentWidget()->height());
  auto widthHeight = cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize();
  if(widthHeight.x()!= 0 && widthHeight.y()!=0)
  {
    double xSizeFull = widthHeight.x();
    double ySizeFull = widthHeight.y();
    double relativeX = (double) this->parentWidget()->x() /  xSizeFull;
    double relativeY = (double) this->parentWidget()->y() /  ySizeFull;
    double relativeWidth = (double) this->parentWidget()->width() / xSizeFull;
    double relativeHeight = (double) this->parentWidget()->height() / ySizeFull;
//    std::cout<<"Writing a Widget configuration: x position is: " << this->parentWidget()->x() << " , the width is: " << xSizeFull << " netting a relative position of: " << relativeX << std::endl;
    root.put("position_relative_x",relativeX);
    root.put("position_relative_y",relativeY);
    root.put("width_relative",relativeWidth);
    root.put("height_relative",relativeHeight);
  }

  root.put_child("data_list", serialize(this->mDataList));

  cedar::aux::ConfigurationNode plot_settings;
  for (int row = 0; row < this->mpLayout->rowCount(); ++row)
  {
    for (int col = 0; col < this->mpLayout->columnCount(); ++col)
    {
      auto layout_item = this->mpLayout->itemAtPosition(row, col);
      if (auto widget_item = dynamic_cast<QWidgetItem*>(layout_item))
      {
        // test if the widget contains a plot
        auto widget = widget_item->widget();
        if (widget->layout())
        {
          for (int i = 0; i < widget->layout()->count(); ++i)
          {
            auto item = dynamic_cast<QWidgetItem*>(widget->layout()->itemAt(i));
            if (!item)
            {
              continue;
            }
            if (auto plot = dynamic_cast<cedar::aux::gui::PlotInterface*>(item->widget()))
            {
              cedar::aux::ConfigurationNode cfg, plot_cfg;
              cfg.put("row", row);
              cfg.put("col", col);
              plot->writeConfiguration(plot_cfg);
              if (!plot_cfg.empty())
              {
                cfg.push_back(cedar::aux::ConfigurationNode::value_type("plot configuration", plot_cfg));
              }
              plot_settings.push_back(cedar::aux::ConfigurationNode::value_type("", cfg));
              break;
            }
          }
        }
      }
    }
  }

  if (!plot_settings.empty())
  {
    root.put_child("plot configurations", plot_settings);
  }
}


std::string cedar::proc::gui::PlotWidget::getWidgetLabel() const
{
  return this->mWidgetLabel;
}

void cedar::proc::gui::PlotWidget::setWidgetLabel(std::string label)
{
  this->mWidgetLabel = label;
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
void cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(const cedar::aux::ConfigurationNode& node, cedar::proc::gui::Connectable* pConnectable, std::string label)
{

  //!@todo These should really check if these nodes exist; if one doesn't, this will lead to trouble...
  int width = node.get<int>("width");
  int height = node.get<int>("height");
  int x = node.get<int>("position_x");
  int y = node.get<int>("position_y");


  auto relX_iter = node.find("position_relative_x");
  auto relY_iter = node.find("position_relative_y");
  auto relW_iter = node.find("width_relative");
  auto relH_iter = node.find("height_relative");

  if (relX_iter != node.not_found() && relY_iter != node.not_found()  && relW_iter != node.not_found()  && relH_iter != node.not_found() )
  {
//    std::cout<<"We loaded an architecture with relative Plot positions!" <<std::endl;
    auto widthHeight = cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize();
    if(widthHeight.x()!= 0 && widthHeight.y()!=0)
    {
//      std::cout<<"Old!!! values are: x,y,width, height: "<< x <<", "<< y << ", " << width << ", "<< height <<std::endl;
      x = (int) (node.get<double>("position_relative_x") * widthHeight.x());
      y = (int) (node.get<double>("position_relative_y") * widthHeight.y());
      // Scaled Width and Heigth only might look pretty dump
      int newWidth = (int) (node.get<double>("width_relative") * widthHeight.x());
      int newHeight = (int) (node.get<double>("height_relative") * widthHeight.y());

      //What about some heuristics that keep the aspect ratio! Todo: Defining very small Plotwidgets results in bad scaling, this needs to be fixed!
      double oldAspectRatio = (double) width / (double) height;
      // Keep the big side and alter the smaller one to keep aspect ratio
      if(newWidth<newHeight)
      { // Width smaller than Height
        height = newHeight;
        width = oldAspectRatio * height;
      }
      else
      { // Height smaller than Width or same
        width = newWidth;
        height = (int)( (double)width / oldAspectRatio);
      }
//      std::cout<<"New values are: x, y, width, height: "<< x <<", "<< y << ", " << width << ", "<< height <<std::endl;
    }
    else
    {
      std::cout<<" The Ide dimensions were somehow 0 in one direction" << std::endl;
    }
  }

  
  auto serialized_data_list = node.get_child("data_list");
  cedar::proc::ElementDeclaration::DataList data_list;
  
  for (auto data_item : serialized_data_list)
  {
    auto p_plot_data = cedar::proc::PlotDataPtr(new cedar::proc::PlotData());
    p_plot_data->readConfiguration(data_item.second);
    data_list.push_back(p_plot_data);
  }
  
  auto p_plot_widget = new cedar::proc::gui::PlotWidget(pConnectable->getConnectable(), data_list);
  if(label!="")
  {
    p_plot_widget->setWidgetLabel(label);
  }
  pConnectable->addPlotWidget(p_plot_widget, x, y, width, height);


  //!@todo This method (and writeConfiguration) are way too tied into the layout of the widget. Just slight changes to the structure will break everything. Fix this!
  auto settings_iter = node.find("plot configurations");
  if (settings_iter != node.not_found())
  {
    for (auto cfg_iter : settings_iter->second)
    {
      auto cfg = cfg_iter.second;
      auto row_iter = cfg.find("row");
      auto col_iter = cfg.find("col");
      if (row_iter == cfg.not_found() || col_iter == cfg.not_found())
      {
        continue;
      }
      int row = row_iter->second.get_value<int>();
      int col = col_iter->second.get_value<int>();

      auto widget_item = dynamic_cast<QWidgetItem*>(p_plot_widget->mpLayout->itemAtPosition(row, col));
      if (!widget_item)
      {
        continue;
      }

      auto layout = widget_item->widget()->layout();
      if (!layout)
      {
        continue;
      }

      for (int i = 0; i < layout->count(); ++i)
      {
        auto item = dynamic_cast<QWidgetItem*>(layout->itemAt(i));
        if (!item)
        {
          continue;
        }

        if (auto plot = dynamic_cast<cedar::aux::gui::PlotInterface*>(item->widget()))
        {
          auto plot_cfg_iter = cfg.find("plot configuration");
          if (plot_cfg_iter != cfg.not_found())
          {
            plot->readConfiguration(plot_cfg_iter->second);
          }
        }
      }
    }
  }
}
