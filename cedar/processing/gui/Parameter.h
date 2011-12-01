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

    File:        Parameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PARAMETER_H
#define CEDAR_PROC_GUI_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <QWidget>


/*!@brief An abstract base widget for graphical representation of Parameter.
 *
 * @todo Write more detailed description of the class here.
 */
class cedar::proc::gui::Parameter : public QWidget
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
  Parameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~Parameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set the parameter, which is graphically represented by this instance
  void setParameter(cedar::aux::ParameterPtr pParameter);
  //!@brief get the parameter, which is graphically represented by this instance
  cedar::aux::ParameterPtr getParameter();

signals:
  //!@brief this signal is emitted whenever a new parameter is linked to this graphical representation
  void parameterPointerChanged();
  //!@brief this signal is emitted if the height of the graphical representation changed due to size changes of
  //        the represented parameter
  void heightChanged();

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
  //!@brief parameter, which is graphically represented by this instance
  cedar::aux::ParameterPtr mParameter;
}; // class cedar::proc::gui::Parameter

#endif // CEDAR_PROC_GUI_PARAMETER_H

