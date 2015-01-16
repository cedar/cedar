/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_OBJECT_LIST_PARAMETER_H
#define CEDAR_AUX_GUI_OBJECT_LIST_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/ObjectListParameter.h"
#include "cedar/auxiliaries/casts.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ObjectListParameter.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <QPushButton>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <string>


/*!@brief User interface representation of cedar::aux::ObjectListParameter.
 */
class cedar::aux::gui::ObjectListParameter : public cedar::aux::gui::Parameter
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectListParameter();

  //!@brief Destructor
  ~ObjectListParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::ConfigurablePtr getSelectedInstance();

  std::string getSelectedType() const;

  inline cedar::aux::ObjectListParameterPtr getObjectList()
  {
    return cedar::aux::asserted_pointer_cast<cedar::aux::ObjectListParameter>(this->getParameter());
  }

  void slotObjectAdded(int index);

  void slotObjectRemoved(int index);

  void appendObjectToInstanceList(int index);

  QString prettyTypeId(const QString& typeId) const;

private slots:
  void parameterPointerChanged();

  void addClicked();

  void removeClicked();

  void currentInstanceIndexChanged(int index);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Combo box for selecting the type to add.
  QComboBox *mpTypeSelector;

  //! Combo box for selecting the instance.
  QComboBox *mpInstanceSelector;

  //! Button for adding a new child
  QPushButton *mpAddButton;

  //! Button for removing a child
  QPushButton *mpRemoveButton;

  //! Connection to the parameters instance added signal.
  boost::signals2::connection mObjectAddedConnection;

  //! Connection to the parameters instance added signal.
  boost::signals2::connection mObjectRemovedConnection;

}; // class cedar::aux::gui::ObjectListParameter

#endif // CEDAR_AUX_GUI_OBJECT_LIST_PARAMETER_H

