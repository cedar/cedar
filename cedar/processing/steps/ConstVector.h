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

    File:        ConstVector.h

    Maintainer: Raul Grieben
    Email:      Raul.Grieben@ini.rub.e
    Date:       2017 08 07

    Description: Processing step, which projects neuronal activation between processing steps of different
                 dimensionality.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CONSTVECTOR_H
#define CEDAR_PROC_STEPS_CONSTVECTOR_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/DoubleVectorParameter.h>
#include <cedar/auxiliaries/MatData.h>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/ConstVector.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::ConstVector : public cedar::proc::Step
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
  ConstVector();

  //--------------------------------------------------------------------------------------------------------------------
  // public slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //@called when the vector dimension changes
  void vectorDimensionChanged();
  void updateVector();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //output vector
  cedar::aux::MatDataPtr mOutput;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //Parameter for the dimension of the output vector
  cedar::aux::UIntParameterPtr _mOutputDimension;
  cedar::aux::DoubleVectorParameterPtr _mValues;

}; // class cedar::proc::steps::ConstVector

#endif // CEDAR_PROC_STEPS_CONSTVECTOR_H
