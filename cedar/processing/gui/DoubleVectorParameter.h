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

    File:        DoubleVectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_DOUBLE_VECTOR_PARAMETER_H
#define CEDAR_PROC_GUI_DOUBLE_VECTOR_PARAMETER_H

// LOCAL INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QDoubleSpinBox>


/*!@brief A widget for manipulating cedar::aux::DoubleVectorParameters.
 */
class cedar::proc::gui::DoubleVectorParameter : public cedar::proc::gui::Parameter
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
  DoubleVectorParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~DoubleVectorParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  /*!@brief Slot that reacts to a change of the parameter pointer.
   */
  void parameterPointerChanged();

  /*!@brief Slot that reacts to a change of the value the parameter.
   */
  void valueChanged(double value);

  /*!@brief Handles changes in the displayed parameter's properties, e.g., a resizing of the vector.
   */
  void propertyChanged();

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
  //! Vector of the spinboxes used for displaying and manipulating the parameter values.
  std::vector<QDoubleSpinBox*> mSpinboxes;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::DoubleVectorParameter

#endif // CEDAR_PROC_GUI_DOUBLE_VECTOR_PARAMETER_H

