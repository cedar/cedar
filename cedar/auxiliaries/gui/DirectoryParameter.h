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

    File:        DirectoryParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_DIRECTORY_PARAMETER_H
#define CEDAR_AUX_GUI_DIRECTORY_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/gui/namespace.h"

// SYSTEM INCLUDES
#include <QLineEdit>


/*!@brief A widget for a cedar::aux::DirectoryParameter.
 */
class cedar::aux::gui::DirectoryParameter : public cedar::aux::gui::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DirectoryParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~DirectoryParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  /*!@brief Handles a click on the browse button.
   */
  void onBrowseClicked();

  /*!@brief Handles a changed parameter pointer.
   */
  void parameterPointerChanged();

  /*!@brief Handles a change in the value of the parameter.
   */
  void parameterValueChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! QLineEdit used for displaying the parameter value.
  QLineEdit *mpEdit;

}; // class cedar::aux::gui::DirectoryParameter

#endif // CEDAR_AUX_GUI_DIRECTORY_PARAMETER_H
