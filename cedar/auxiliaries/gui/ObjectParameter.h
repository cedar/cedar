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

    File:        ObjectParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_OBJECT_PARAMETER_H
#define CEDAR_AUX_GUI_OBJECT_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <QPushButton>
#include <boost/signals2.hpp>


/*!@brief User interface representation of cedar::aux::ObjectListParameter.
 *
 * @todo React to a changed value in the parameter
 */
class cedar::aux::gui::ObjectParameter : public cedar::aux::gui::Parameter
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
  ObjectParameter();

  //!@brief Destructor
  ~ObjectParameter();

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
  std::string getSelectedType() const;

  inline cedar::aux::ObjectParameterPtr getObjectParameter()
  {
    //!@todo make this an asserted cast, because it should never fail.
    return cedar::aux::asserted_pointer_cast<cedar::aux::ObjectParameter>(this->getParameter());
  }

private slots:
  void parameterPointerChanged();

  void editClicked();

  void currentTypeChanged(int index);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Combo box for selecting the type to add.
  QComboBox *mpTypeSelector;

  //! Button for removing a child
  QPushButton *mpEditButton;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::ObjectParameter

#endif // CEDAR_AUX_GUI_OBJECT_PARAMETER_H

