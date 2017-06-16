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

    File:        VectorToScalars.h

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 05 19

    Description: Header file for the class cedar::proc::steps::VectorToScalars.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_VECTOR_TO_SCALARS_H
#define CEDAR_PROC_STEPS_VECTOR_TO_SCALARS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/VectorToScalars.fwd.h"

// SYSTEM INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/UIntParameter.h>
#include <vector>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::VectorToScalars  : public cedar::proc::Step
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
    VectorToScalars();
  
    //--------------------------------------------------------------------------------------------------------------------
    // public slots
    //--------------------------------------------------------------------------------------------------------------------
  public slots:
    //@called when the vector dimension changes
    void vectorDimensionChanged();
    void inputConnectionChanged(const std::string &inputName);

    //--------------------------------------------------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------------------------------------------------

public:
    //!@brief input verification
    cedar::proc::DataSlot::VALIDITY determineInputValidity
    (
      cedar::proc::ConstDataSlotPtr slot,
      cedar::aux::ConstDataPtr data
    )const;
  
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

    //input vector
    cedar::aux::ConstMatDataPtr mInput;

    //output scalars
    std::vector< cedar::aux::MatDataPtr > mOutputs;
  
    // returns i-th slots name
    std::string makeSlotName(const int i);
  
    //--------------------------------------------------------------------------------------------------------------------
    // parameters
    //--------------------------------------------------------------------------------------------------------------------
  protected:
    // none yet
  
  private:
    //Parameter for the dimension of the output vector
    cedar::aux::UIntParameterPtr _mInputDimension;

}; // class cedar::proc::steps::VectorToScalars

#endif // CEDAR_PROC_STEPS_VECTOR_TO_SCALARS_H

