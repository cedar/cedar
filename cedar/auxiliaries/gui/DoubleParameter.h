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

    File:        DoubleParameter.h

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

#ifndef CEDAR_AUX_GUI_DOUBLE_PARAMETER_H
#define CEDAR_AUX_GUI_DOUBLE_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/DoubleParameter.fwd.h"

// SYSTEM INCLUDES
#include <QDoubleSpinBox>

//----------------------------------------------------------------------------------------------------------------------
// template specialization for QSpinBox
//----------------------------------------------------------------------------------------------------------------------
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void NumericWidgetPolicy<double, QDoubleSpinBox>::setPrecision(QDoubleSpinBox* pWidget, int precision)
      {
        pWidget->setDecimals(precision);
      }
      //!@endcond
    }
  }
}

/*!@brief Widget for manipulating cedar::aux::DoubleParameters.
 */
class cedar::aux::gui::DoubleParameter : public cedar::aux::gui::NumericParameter<double, QDoubleSpinBox>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::NumericParameter<double, QDoubleSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DoubleParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~DoubleParameter();

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
  void parameterChanged();

private slots:
  /*!@brief Reacts to a change of the parameter's value.
   */
  void valueChanged(double value);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::DoubleParameter

#endif // CEDAR_AUX_GUI_DOUBLE_PARAMETER_H
