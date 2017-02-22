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

    File:        FFTWPlanningStrategy.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 01 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_FFTW_PLANNING_STRATEGY_H
#define CEDAR_AUX_CONV_FFTW_PLANNING_STRATEGY_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/convolution/FFTWPlanningStrategy.fwd.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_FFTW
#include <fftw3.h>
#endif


/*!@brief Enum describing the convolution mode.
 *
 *        This parameter describes the planning strategy of the FFTW convolution:
 *        <ul>
 *          <li>@em Estimate: Use a heuristic.</li>
 *          <li>@em Measure: Use time measures of different FFT methods.</li>
 *          <li>@em Patient: Use time measures of different FFT methods, search longer.</li>
 *          <li>@em Exhaustive: Use time measures of different FFT methods, search longest.</li>
 *        </ul>
 */
class cedar::aux::conv::FFTWPlanningStrategy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The enum id
  typedef cedar::aux::EnumId Id;

  //! Pointer type to the enum base object of this class.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Initialization of the enum values.
  static void construct();

  //! Returns a reference to the base enum object.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the base enum object.
  static const cedar::aux::conv::FFTWPlanningStrategy::TypePtr& typePtr();

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
public:
#ifdef CEDAR_USE_FFTW
  //! Use heuristics.
  static const Id Estimate = FFTW_ESTIMATE;

  //! Use time measures of different FFT methods.
  static const Id Measure = FFTW_MEASURE;

  //! Use time measures of different FFT methods, search longer.
  static const Id Patient = FFTW_PATIENT;

  //! Use time measures of different FFT methods, search longest.
  static const Id Exhaustive = FFTW_EXHAUSTIVE;
#else
  //! Use heuristics.
  static const Id Estimate = 0;

  //! Use time measures of different FFT methods.
  static const Id Measure = 1;

  //! Use time measures of different FFT methods, search longer.
  static const Id Patient = 2;

  //! Use time measures of different FFT methods, search longest.
  static const Id Exhaustive = 3;
#endif

protected:
  // none yet
private:
  //! The type object for this enum class.
  static cedar::aux::EnumType<cedar::aux::conv::FFTWPlanningStrategy> mType;

}; // class cedar::aux::conv::FFTWPlanningStrategy

#endif // CEDAR_AUX_CONV_FFTW_PLANNING_STRATEGY_H
