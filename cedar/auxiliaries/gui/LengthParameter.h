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

    File:        LengthParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 03 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_LENGTH_PARAMETER_H
#define CEDAR_AUX_GUI_LENGTH_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericParameter.h"
#include "cedar/auxiliaries/LengthParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/LengthParameter.fwd.h"

// SYSTEM INCLUDES
#include <QDoubleSpinBox>
#ifndef Q_MOC_RUN
  #include <boost/units/io.hpp>
#endif // Q_MOC_RUN

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
      inline void NumericWidgetPolicy<cedar::unit::Length, QDoubleSpinBox>::setPrecision(QDoubleSpinBox* pWidget, int precision)
      {
        pWidget->setDecimals(precision);
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Length, QDoubleSpinBox>::setValue(QDoubleSpinBox* pWidget, const cedar::unit::Length& newValue)
      {
        pWidget->setValue(newValue/cedar::unit::meters);
        pWidget->setSuffix(" m");
      }

      template<>
      inline cedar::unit::Length NumericWidgetPolicy<cedar::unit::Length, QDoubleSpinBox>::getValue(QDoubleSpinBox* pWidget)
      {
        return pWidget->value() * cedar::unit::meters;
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Length, QDoubleSpinBox>::setMaximum(QDoubleSpinBox* pWidget, const cedar::unit::Length& newValue)
      {
        pWidget->setMaximum(newValue/cedar::unit::meters);
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Length, QDoubleSpinBox>::setMinimum(QDoubleSpinBox* pWidget, const cedar::unit::Length& newValue)
      {
        pWidget->setMinimum(newValue/cedar::unit::meters);
      }
      //!@endcond
    }
  }
}

/*!@brief Widget for manipulating cedar::aux::LengthParameters.
 */
class cedar::aux::gui::LengthParameter : public cedar::aux::gui::NumericParameter<cedar::unit::Length, QDoubleSpinBox>
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
  LengthParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~LengthParameter();

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

}; // class cedar::aux::gui::LengthParameter

#endif // CEDAR_AUX_GUI_LENGTH_PARAMETER_H
