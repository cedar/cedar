/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        PropertyPane.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROPERTY_PANE_H
#define CEDAR_PROC_PROPERTY_PANE_H

// LOCAL INCLUDES
#include "processing/gui/namespace.h"
#include "processing/namespace.h"
#include "auxiliaries/TypeBasedFactory.h"
#include "processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QTableWidget>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::PropertyPane : public QTableWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

private:
  typedef
      cedar::aux::TypeBasedFactory
      <
        cedar::aux::Parameter,
        cedar::proc::gui::Parameter,
        cedar::aux::ParameterPtr
      >
      DataWidgetTypes;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PropertyPane(QWidget *pParent = NULL);

  //!@brief Destructor
  ~PropertyPane();

  //!@todo change from step to configuragle when the change is made in cedar::processing.
  void display(cedar::proc::StepPtr pStep);

  void display(cedar::proc::TriggerPtr pTrigger);

  void display(cedar::aux::ConfigurablePtr pConfigurable);

  void resetContents();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  DataWidgetTypes& dataWidgetTypes();

public slots:
  void resetPointer();

  void rowSizeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void append(cedar::proc::Step::ParameterList& parameters);

  void addHeadingRow(const std::string& label);

  void addLabelRow(const std::string& label);

  void addPropertyRow(cedar::aux::ParameterPtr parameter);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  static DataWidgetTypes mDataWidgetTypes;
  //!@todo this should be one configurable pointer
  boost::weak_ptr<cedar::aux::Configurable> mDisplayedConfigurable;

  std::map<cedar::proc::gui::Parameter*, int> mParameterRowIndex;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::PropertyPane

#endif // CEDAR_PROC_PROPERTY_PANE_H

