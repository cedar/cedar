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

#ifndef CEDAR_AUX_GUI_PARAMETER_H
#define CEDAR_AUX_GUI_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Parameter.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>


/*!@brief An abstract base widget for graphical representation of cedar::aux::Parameters.
 */
class cedar::aux::gui::Parameter : public QWidget
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

  //! Makes the parameter read-only. Child classes can override this to provide specific read-only variants of their widgets.
  virtual void setReadOnly(bool readOnly);

signals:
  //!@brief this signal is emitted whenever a new parameter is linked to this graphical representation
  void parameterPointerChanged();

  /*!@brief this signal is emitted if the height of the graphical representation changed due to size changes of
   *        the represented parameter
   */
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

private slots:
  /*!@brief Reacts to a change of the parameter pointer.
   *
   * @remarks This method is implemented here because it is overridden in derived classes which cannot have slots
   *          because they are templated (which is not supported by Qt).
   */
  virtual void parameterChanged();

  /*!@brief Reacts to a change of the parameter's properties.
   *
   * @remarks This method is implemented here because it is overridden in derived classes which cannot have slots
   *          because they are templated (which is not supported by Qt).
   */
  virtual void propertiesChanged();

  /*! Slot that can be overridden when the value of the connected parameter changes.
   */
  virtual void valueChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief parameter, which is graphically represented by this instance
  cedar::aux::ParameterPtr mParameter;
}; // class cedar::aux::gui::Parameter

CEDAR_AUX_EXPORT_SINGLETON(cedar::aux::gui::ParameterFactory);

#endif // CEDAR_AUX_GUI_PARAMETER_H
