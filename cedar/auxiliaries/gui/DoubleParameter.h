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
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/gui/IgnoreLocaleDoubleSpinBox.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/DoubleParameter.fwd.h"

// SYSTEM INCLUDES
#include <QDoubleSpinBox>
#include <cmath>

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
      inline void NumericWidgetPolicy<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::setPrecision(cedar::aux::gui::IgnoreLocaleDoubleSpinBox* pWidget, int precision)
      {
        pWidget->setDecimals(precision);
      }
      //!@endcond

//      //!@cond SKIPPED_DOCUMENTATION
//      template<>
//      inline void NumericWidgetPolicy<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::manuallyChangedValue(cedar::aux::gui::IgnoreLocaleDoubleSpinBox* pWidget, const double& newvalue, const double& oldvalue)
//      {
//        if( newvalue != oldvalue )
//        {
//          // lets try this: set the last manual diff as the step size
//          auto diff = std::fabs( newvalue - oldvalue );
//
//          // find the magnitude of the last change. set that to be the step
//          double newstep;
//
//          double magnitude_newval = std::floor( std::log10( newvalue ) );
//          double magnitude_oldval = std::floor( std::log10( oldvalue ) );
//          double magnitude_diff =  std::floor( std::log10( diff ) + 1e-8 );
//                                            // avoid numerical issues
//                                            // with smaller increases than intended ...
//
//          double magnitude;
//
//          if (std::isnan(magnitude_newval)
//              || std::isinf(magnitude_newval)) // when manually editing to 0
//          {
//            newstep= pWidget->singleStep(); // keep
//          }
//          else
//          {
//            if (magnitude_newval > magnitude_oldval
//                && std::fabs( magnitude_newval - magnitude_diff ) <= 1.0)
//            {
//              magnitude= magnitude_newval;
//                // (because the magnitude of the diff will be one smaller
//                //  because of the floor())
//            }
//            else
//            {
//              magnitude= magnitude_diff;
//            }
//
//            newstep= std::pow( 10, magnitude );
//          }
//
//          if (newstep != pWidget->singleStep()
//              && !cedar::aux::math::isZero( newstep ))
//          {
//            pWidget->setSingleStep( newstep );
//          }
//        }
//      }
      //!@endcond

    }
  }
}

/*!@brief Widget for manipulating cedar::aux::DoubleParameters.
 */
class cedar::aux::gui::DoubleParameter : public cedar::aux::gui::NumericParameter<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::NumericParameter<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox> Base;

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
