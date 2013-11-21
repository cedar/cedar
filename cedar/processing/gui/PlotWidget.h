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

    File:        PlotWidget.h

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.rub.de
    Date:        2013 07 31

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PLOT_WIDGET_H
#define CEDAR_PROC_GUI_PLOT_WIDGET_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/PlotWidget.fwd.h"
#include "cedar/processing/gui/StepItem.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QGridLayout>
#include <QLabel>

/*!@todo Document this.
 */
class cedar::proc::gui::PlotWidget : public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PlotWidget(
    cedar::proc::StepPtr step,
    const cedar::proc::ElementDeclaration::DataList& data
  );

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct LabeledPlot
  {
    // constructor
    LabeledPlot(QLabel* pLabel, cedar::aux::gui::ConstPlotDeclarationPtr pPlotDecl = cedar::aux::gui::ConstPlotDeclarationPtr())
    :
    mpPlotDeclaration(pPlotDecl),
    mpLabel(pLabel),
    mpPlotter(NULL)
    {
      if(mpPlotDeclaration)
      {
        mpPlotter = mpPlotDeclaration->createPlot();
      }
    };
    // members
    cedar::aux::gui::ConstPlotDeclarationPtr mpPlotDeclaration;
    QLabel * mpLabel;
    cedar::aux::gui::PlotInterface* mpPlotter;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get number of rows
  int getRowCount();

  //!@brief get number of columns
  int getColumnCount();

  //!@brief write plot configuration to a configuration node
  void writeConfiguration(cedar::aux::ConfigurationNode& root);

  //!@brief returns name of step plotted by this plot
  static const std::string getStepNameFromConfiguration(const cedar::aux::ConfigurationNode& node)
  {
    return node.get<std::string>("step");
  }

  //!@brief recover plot from configuration
  static void createAndShowFromConfiguration(const cedar::aux::ConfigurationNode& node, cedar::proc::gui::StepItem* pStepItem);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void fillGridWithPlots();
  bool createAndAddPlotToGrid(cedar::aux::gui::ConstPlotDeclarationPtr decl, cedar::aux::DataPtr pData, const std::string& title, int row, int column);
  bool tryAppendDataToPlot(cedar::aux::DataPtr pData, const std::string& title);
  void removePlotForExternalData(cedar::aux::ConstDataPtr data);
  cedar::aux::ConfigurationNode serialize(const cedar::proc::ElementDeclaration::DataList& dataList) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::ElementDeclaration::DataList mData;
  cedar::proc::StepPtr mStep;
  LabeledPlot mLabeledPlot;
  int mGridSpacing;
  int mColumns;
  QGridLayout* mpLayout;

}; // cedar::proc::gui::PlotWidget

#endif // CEDAR_PROC_GUI_PLOT_WIDGET_H
