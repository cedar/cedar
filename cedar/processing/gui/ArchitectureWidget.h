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

    File:        ArchitectureWidget.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 05 08

    Description: Header file for the class cedar::proc::gui::ArchitectureWidget.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_H
#define CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ArchitectureWidget.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/auxiliaries/Data.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QPushButton>
#include <map>
#include <string>
#include <utility>
#include <vector>


/*!@brief A widget for opening a set of plots for an architecture based on a configuration file.
 */
class cedar::proc::gui::ArchitectureWidget : public QWidget, public cedar::aux::Configurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct BaseCellAttributes
  {
    BaseCellAttributes(int rowSpan = 1, int mColumnSpan = 1, int mRowStretch = 1, int mColumStretch = 1)
    :
    mRowSpan(rowSpan),
    mColumnSpan(mColumnSpan),
    mRowStretch(mRowStretch),
    mColumnStretch(mColumStretch)
    {
    }

    //! Determines how many cells an entry spans.
    int mRowSpan, mColumnSpan;
    //! Weight that determines how much an entry stretches in the layout.
    int mRowStretch, mColumnStretch;
  };

  struct StepActionParameters
  {
    std::vector<std::pair<std::string, std::string> > mStepActions;
    bool mConfirm;
  };
  CEDAR_GENERATE_POINTER_TYPES(StepActionParameters);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  ArchitectureWidget(cedar::proc::GroupPtr group, QWidget* pParent = nullptr);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  void writeConfiguration(cedar::aux::ConfigurationNode& node) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  template <typename T>
  static T readOptional(const cedar::aux::ConfigurationNode& node, const std::string& key, const T& defaultValue);

  cedar::aux::ConstDataPtr findData(const std::string& path) const;

  void readDataNode(const cedar::aux::ConfigurationNode& node, std::string& dataPath, std::string& title);

  QWidget* readPlot(const cedar::aux::ConfigurationNode& entry, int row, int column);

  QWidget* readLabel(const cedar::aux::ConfigurationNode& entry);

  QWidget* readStepAction(const cedar::aux::ConfigurationNode& entry);

  void addEntry(const cedar::aux::ConfigurationNode& entry, int rowOffset = 0, int colOffset = 0, const BaseCellAttributes& attributes = BaseCellAttributes());

  void addTemplate(int row, int column, const cedar::aux::ConfigurationNode& entry, const BaseCellAttributes& attributes = BaseCellAttributes());

  void readTemplates(const cedar::aux::ConfigurationNode& templates);

  void applySubstitutions(cedar::aux::ConfigurationNode& target, const cedar::aux::ConfigurationNode& substitutions);

  void readConfig(const cedar::aux::ConfigurationNode& config);

private slots:
  void triggerStepAction();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::map<std::string, cedar::aux::ConfigurationNode> mTemplates;

  std::map<QPushButton*, StepActionParametersPtr> mStepActionParameters;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::proc::GroupPtr mGroup;

}; // class cedar::proc::gui::ArchitectureWidget

#endif // CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_H

