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

    File:        ShiftedMultiplication.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2016 01 08

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_SHIFTED_MULTIPLICATION_H
#define CEDAR_PROC_STEPS_SHIFTED_MULTIPLICATION_H

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/UIntParameter.fwd.h>
#include <cedar/auxiliaries/MatData.fwd.h>

// FORWARD DECLARATIONS
#include <cedar/processing/steps/ShiftedMultiplication.fwd.h>

// SYSTEM INCLUDES


/*!@brief A processing step that a three-dimensional activation pattern by multiplying two two-dimensional patterns with a fixed shift and all possible angles.
 */
class cedar::proc::steps::ShiftedMultiplication : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ShiftedMultiplication();

  //!@brief Destructor
  ~ShiftedMultiplication();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void compute(const cedar::proc::Arguments&);

public slots:
  void recompute();
  void reconfigure();
  void inputConnectionChanged(const std::string&);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // inputs
  cedar::aux::MatDataPtr mToward;
  cedar::aux::MatDataPtr mAway;

  // outputs
  cedar::aux::MatDataPtr mOutput;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::UIntParameterPtr _mDistance;
  cedar::aux::UIntParameterPtr _mOrientationSize;
}; // class cedar::proc::steps::ShiftedMultiplication

#endif // CEDAR_PROC_STEPS_SHIFTED_MULTIPLICATION_H
