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

    File:        UIntVectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_UINT_VECTOR_PARAMETER_H
#define CEDAR_PROC_GUI_UINT_VECTOR_PARAMETER_H

// LOCAL INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QSpinBox>


/*!@brief Widget for manipulating vectors of unsigned integer values.
 *
 * @todo This should be abstracted, probably in a template class:
 *       template <class ParameterType, class WidgetType> NumericVectorParameter,
 *       where, e.g., ParameterType = UIntVector and WidgetType = QSpinBox. Otherwise, a lot of code might get
 *       duplicated
 */
class cedar::proc::gui::UIntVectorParameter : public cedar::proc::gui::Parameter
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
  UIntVectorParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~UIntVectorParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  //!@brief handles a change of the associated parameters
  void parameterPointerChanged();

  //!@brief handles a change in the parameters
  void valueChanged(int value);

  //!@brief Handles changes in the displayed parameter's properties, e.g., a resizing of the vector.
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
  //!@brief a vector of spinboxes for displaying and changing the associated parameters
  std::vector<QSpinBox*> mSpinboxes;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::UIntVectorParameter

#endif // CEDAR_PROC_GUI_UINT_VECTOR_PARAMETER_H

