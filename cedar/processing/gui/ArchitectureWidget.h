/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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


/*!@brief A widget for opening a set of plots for an architecture based on a configuration file.
 */
class cedar::proc::gui::ArchitectureWidget : public QWidget, public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

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

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::proc::GroupPtr mGroup;

}; // class cedar::proc::gui::ArchitectureWidget

#endif // CEDAR_PROC_GUI_ARCHITECTURE_WIDGET_H
