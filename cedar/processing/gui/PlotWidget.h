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
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <tuple>
#include <utility>

//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace PlotWidgetPrivate
      {
        // This should be a private nested class, but that is not supported by the MOC.
        class LabeledPlot : public QObject
        {
          Q_OBJECT
        public:
          // constructor
          LabeledPlot
          (
            const QString& pLabel,
            cedar::aux::ConstDataPtr pData,
            cedar::proc::PlotDataPtr pPlotData,
            const std::string& declarationToUse = std::string()
          );

        private:
          void fillPlotOptions(QMenu* menu);

          void openPlotFromDeclaration(const std::string& declarationToFind);

        private slots:
          void openDefaultPlot();

          void openSpecificPlot();

        public:
          // members
          cedar::aux::gui::ConstPlotDeclarationPtr mpPlotDeclaration;
          QPushButton* mpPlotSelector;
          QWidget* mpPlotContainer;
          cedar::aux::gui::PlotInterface* mpPlotter;
          QHBoxLayout* mpTitleLayout;
          bool mIsMultiPlot;
          cedar::aux::ConstDataPtr mpData;
          cedar::proc::PlotDataPtr mpPlotData;
          std::string mTitle;

          std::map<cedar::aux::ConstDataPtr, const std::string> mMultiPlotData;
          QLabel* mpLabel;

        private:
          int mGridMinimumHeight;
          int mGridMinimumWidth;
        };
        CEDAR_GENERATE_POINTER_TYPES(LabeledPlot);
      }
    }
  }
}
//!@endcond

/*!@brief a widget for plots
 */
class cedar::proc::gui::PlotWidget : public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  // pretend this is a nested class...
  typedef cedar::proc::gui::PlotWidgetPrivate::LabeledPlot LabeledPlot;
  CEDAR_GENERATE_POINTER_TYPES(LabeledPlot);

  typedef std::pair<cedar::aux::ConstDataPtr, LabeledPlotPtr> PlotGridMapItem;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PlotWidget(
    cedar::proc::ConnectablePtr connectable,
    const cedar::proc::ElementDeclaration::DataList& data
  );

  //!@brief The standard destructor
  ~PlotWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get number of rows
  int getRowCount() const;

  //!@brief get number of columns
  int getColumnCount() const;

  //!@brief write plot configuration to a configuration node
  void writeConfiguration(cedar::aux::ConfigurationNode& root);

  std::string getWidgetLabel() const;

  void setWidgetLabel(std::string label);


  //!@brief returns name of step plotted by this plot
  static const std::string getStepNameFromConfiguration(const cedar::aux::ConfigurationNode& node)
  {
    return node.get<std::string>("step");
  }

  //!@brief recover plot from configuration
  static void createAndShowFromConfiguration(const cedar::aux::ConfigurationNode& node, cedar::proc::gui::Connectable* pConnectable, std::string label = "");

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Fills the Plot Widget's Grid Layout with Plots of the provided data-list
  void fillGridWithPlots();
  //!@brief creates a plot of the given data at a given slot, returns false if it created a new plot-widget, true if it appended to an existing one.
  void processSlot(cedar::aux::ConstDataPtr slot, cedar::proc::PlotDataPtr dataItem, const std::string& title);
  void createAndAddPlotToGrid(const std::string& decl, cedar::aux::ConstDataPtr pData, cedar::proc::PlotDataPtr dataItem, const std::string& title);
  void tryAppendDataToPlot(cedar::aux::ConstDataPtr pData, const std::string& title, LabeledPlotPtr currentLabeledPlot);
  //!@brief gets called if data is added to slot and adds a plot
  void addPlotOfExternalData(cedar::aux::ConstDataPtr pData, cedar::proc::ExternalDataPtr slot, cedar::proc::PlotDataPtr dataItem);
  //!@brief gets called if data is removed from a slot and removes the plot thereof
  void removePlotOfExternalData(cedar::aux::ConstDataPtr pData);
  //!@brief returns the next free grid slot
  std::tuple<int, int> usingNextFreeGridSlot();

  //! Returns (row, column) corresponding to the given LabeledPlot. Note, that this refers to the title row of the plot.
  std::tuple<int, int> findGridPositionOf(LabeledPlotPtr plot);

  cedar::aux::ConfigurationNode serialize(const cedar::proc::ElementDeclaration::DataList& dataList) const;

  //!@brief removes a widget from the grid layout
  void removeFromQGridlayout(QLayoutItem* item);

  //!@brief removes a widget from the grid layout
  void remove_qgridlayout_widget(QWidget* widget);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::ElementDeclaration::DataList mDataList;
  cedar::proc::ConnectablePtr mConnectable;
  int mGridSpacing;
  int mColumns;
  QGridLayout* mpLayout;

  std::vector<boost::signals2::connection> mSignalConnections;
  std::map<cedar::aux::ConstDataPtr, LabeledPlotPtr> mPlotGridMap;
  //!@brief tuples are of this structure: <row, col>
  std::list<std::tuple<int, int>> mFreeGridSlots;

  std::string mWidgetLabel;

}; // cedar::proc::gui::PlotWidget

#endif // CEDAR_PROC_GUI_PLOT_WIDGET_H
