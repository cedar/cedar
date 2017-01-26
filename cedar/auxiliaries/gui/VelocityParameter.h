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

    File:        VelocityParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 03 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VELOCITY_PARAMETER_H
#define CEDAR_AUX_GUI_VELOCITY_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericParameter.h"
#include "cedar/auxiliaries/VelocityParameter.h"
#include "cedar/auxiliaries/LengthParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/VelocityParameter.fwd.h"

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
      inline void NumericWidgetPolicy<cedar::unit::Velocity, QDoubleSpinBox>::setPrecision(QDoubleSpinBox* pWidget, int precision)
      {
        pWidget->setDecimals(precision);
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Velocity, QDoubleSpinBox>::setValue(QDoubleSpinBox* pWidget, const cedar::unit::Velocity& newValue)
      {
        pWidget->setValue(newValue/(cedar::unit::meters/cedar::unit::seconds));
        pWidget->setSuffix(" m s^-1");
      }

      template<>
      inline cedar::unit::Velocity NumericWidgetPolicy<cedar::unit::Velocity, QDoubleSpinBox>::getValue(QDoubleSpinBox* pWidget)
      {
        return pWidget->value() * (cedar::unit::meters/cedar::unit::seconds);
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Velocity, QDoubleSpinBox>::setMaximum(QDoubleSpinBox* pWidget, const cedar::unit::Velocity& newValue)
      {
        pWidget->setMaximum(newValue/(cedar::unit::meters/cedar::unit::seconds));
      }

      template<>
      inline void NumericWidgetPolicy<cedar::unit::Velocity, QDoubleSpinBox>::setMinimum(QDoubleSpinBox* pWidget, const cedar::unit::Velocity& newValue)
      {
        pWidget->setMinimum(newValue/(cedar::unit::meters/cedar::unit::seconds));
      }
      //!@endcond
    }
  }
}

/*!@brief Widget for manipulating cedar::aux::VelocityParameters.
 */
class cedar::aux::gui::VelocityParameter : public cedar::aux::gui::NumericParameter<cedar::unit::Velocity, QDoubleSpinBox>
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
  VelocityParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~VelocityParameter();

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

}; // class cedar::aux::gui::VelocityParameter

#endif // CEDAR_AUX_GUI_VELOCITY_PARAMETER_H
