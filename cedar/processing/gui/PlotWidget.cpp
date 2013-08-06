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
    const cedar::proc::ElementDeclaration::DataList& data,
    cedar::aux::gui::ConstPlotDeclarationPtr declaration
)
:
mData(data),
mpStep(step),
mLabeledPlot({NULL, NULL}),
mpPlotDeclaration(declaration),
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

  for (auto it = mData.begin(); it != mData.end(); ++it)
  {
    auto deit = *it;
    try
    {
      cedar::proc::DataSlotPtr slot = mpStep->getSlot(deit->getParameter<cedar::aux::EnumParameter>("id")->getValue(), deit->getParameter<cedar::aux::StringParameter>("name")->getValue());
      cedar::aux::DataPtr p_data = slot->getData();
      const std::string& title = slot->getText();

      int column = count % mColumns;
      int row = 2 * (count / mColumns);

      if (p_data)
      // skip slots that aren't set
      {
        // the static cast is okay iff mpPlotDeclaration is false
        if (mLabeledPlot.mpPlotter == NULL || mpPlotDeclaration || !cedar::aux::asserted_cast<cedar::aux::gui::DataPlotter*>(mLabeledPlot.mpPlotter)->canAppend(p_data))
        // if no plotter exists or we cannot append the data to the current plotter (which is also the case
        // if the PlotDeclaration is set) we have to create a new plotter and have it plot the data
        {
          count += createAndAddPlotToGrid(p_data, title, row, column) ? 1 : 0;
        }
        else
        {
          is_multiplot = tryAppendDataToPlot(p_data, title);
        }
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      if (!deit->getParameter<cedar::aux::BoolParameter>("ignoreIfMissing"))
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
      if (!deit->getParameter<cedar::aux::BoolParameter>("ignoreIfMissing"))
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Could not plot data. Exception: " + e.exceptionInfo(),
          "cedar::proc::gui::PlotWidget::PlotWidget()"
        );
      }
    }
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
  cedar::aux::DataPtr pData,
  const std::string& title,
  int row,
  int column
)
{
  bool success = true;
  cedar::aux::gui::PlotInterface* data_plotter;
  if(mpPlotDeclaration)
  {
    data_plotter = mpPlotDeclaration->createPlot();
  }
  else
  {
    data_plotter = new cedar::aux::gui::DataPlotter(); 
  }
  mLabeledPlot = LabeledPlot(new QLabel(QString::fromStdString(title)), data_plotter);
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
    static_cast<cedar::aux::gui::DataPlotter*>(mLabeledPlot.mpPlotter)->append(pData, title);
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
  root.put("step", this->mpStep->getName());
  root.put("position_x", this->parentWidget()->x());
  root.put("position_y", this->parentWidget()->y());
  root.put("width", this->parentWidget()->width());
  root.put("height", this->parentWidget()->height());
  if(mpPlotDeclaration)
    {
      root.put("PlotDeclaration", mpPlotDeclaration->getClassName());
    }
    else
    {
      root.put("PlotDeclaration", ""); 
    }
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
  std::string plot_declaration = node.get<std::string>("PlotDeclaration");
  
  auto serialized_data_list = node.get_child("data_list");
  cedar::proc::ElementDeclaration::DataList data_list;
  
  for(auto it = serialized_data_list.begin(); it != serialized_data_list.end(); ++it)
  {
    auto p_plot_data = cedar::proc::PlotDataPtr(new cedar::proc::PlotData());
    p_plot_data->readConfiguration(it->second);
    data_list.push_back(p_plot_data);
  }
  
  auto data = pStepItem->getStep()->getSlot(
                data_list.front()->getParameter<cedar::aux::EnumParameter>("id")->getValue(),
                data_list.front()->getParameter<cedar::aux::StringParameter>("name")->getValue()
              )->getData();
  auto declarations = cedar::aux::gui::PlotDeclarationManagerSingleton::getInstance()->find(data)->getData();
  cedar::aux::gui::ConstPlotDeclarationPtr decl;
  
  for(auto it = declarations.begin(); it != declarations.end(); ++it)
  {
    auto deit = *it;
    if(deit->getClassName() == plot_declaration)
    {
      decl = deit;
    }
  }

  if(!decl)
  {
    decl = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data);
  }

  auto p_plot_widget = new cedar::proc::gui::PlotWidget(pStepItem->getStep(), data_list, decl);
  pStepItem->addPlotWidget(p_plot_widget, x, y, width, height);
}