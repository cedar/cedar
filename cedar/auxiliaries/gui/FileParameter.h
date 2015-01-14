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

    File:        FileParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_FILE_PARAMETER_H
#define CEDAR_AUX_GUI_FILE_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/FileParameter.fwd.h"

// SYSTEM INCLUDES
#include <QLineEdit>
#include <QPushButton>


/*!@brief A widget for a cedar::aux::DirectoryParameter.
 */
class cedar::aux::gui::FileParameter : public cedar::aux::gui::Parameter
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
  FileParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~FileParameter();

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
  void propertiesChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! QLineEdit used for displaying the parameter value.
  QLineEdit* mpEdit;

  //! Button used for opening the dialog for selecting a file.
  QPushButton* mpButton;

}; // class cedar::aux::gui::FileParameter

#endif // CEDAR_AUX_GUI_FILE_PARAMETER_H

