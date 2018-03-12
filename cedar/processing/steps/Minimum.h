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

    File:        

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_MINIMUM_H
#define CEDAR_PROC_STEPS_MINIMUM_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/Minimum.fwd.h"

// SYSTEM INCLUDES


/*!@brief   This is a step that sums up a number of inputs.
 */
class cedar::proc::steps::Minimum : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Minimum();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  /*! A helper function that calculates the sum of all matrices in the given slot.
   *
   * @remarks This function assumes that the output matrix, sum, is initialized to the appropriate size, and that all
   *          matrices in the slot are the same size (0D matrices are treated as scalar additions).
   */
  static void sumSlot(cedar::proc::ExternalDataPtr slot, cv::Mat& sum, bool lock = false);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Method that is called whenever an input is connected to the Connectable.
  virtual void inputConnectionChanged(const std::string& inputName);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The input slot containing all the terms.
  cedar::proc::ExternalDataPtr mInputs;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;

private:
  // none yet
}; // class cedar::proc::steps::Minimum

#endif // CEDAR_PROC_STEPS_MINIMUM_H
