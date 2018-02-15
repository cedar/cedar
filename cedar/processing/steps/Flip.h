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

    File:        Flip.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_FLIP_H
#define CEDAR_PROC_STEPS_FLIP_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/BoolVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/Flip.fwd.h"

// SYSTEM INCLUDES


/*!@brief   This is a flip step
 */
class cedar::proc::steps::Flip : public cedar::proc::Step
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
  Flip();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //!@brief this slot triggers the step and is called every time a parameter changes
  void flipDirectionsChanged();

  /*! Returns whether the given dimension is set to be flipped.
   *
   * @throws cedar::aux::IndexOutOfRangeException if the dimension exceeds the dimension of the input.
   */
  bool isDimensionFlipped(unsigned int dimension) const
       throw (cedar::aux::IndexOutOfRangeException, cedar::aux::FailedAssertionException);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void recompute();

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:
  void readConfiguration(const cedar::aux::ConfigurationNode& configuration);

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Vector of bools allowing the user to select which dimensions are to be flipped.
  cedar::aux::BoolVectorParameterPtr _mFlipDimensions;

private:

}; // class cedar::proc::steps::Flip

#endif // CEDAR_PROC_STEPS_FLIP_H
