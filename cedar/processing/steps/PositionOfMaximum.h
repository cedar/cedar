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

    File:        PositionOfMaximum.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       
    Date:        2017 05 14

    Description: Header file for the class cedar::proc::steps::PositionOfMaximum.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_POSITION_OF_MAXIMUM_H
#define CEDAR_PROC_STEPS_POSITION_OF_MAXIMUM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/PositionOfMaximum.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::PositionOfMaximum : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class UnitType
  {
    public:
      //! the id of an enum entry
      typedef cedar::aux::EnumId Id;

      //! constructs the enum for all ids
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(Maximum, "Maximum"));
        mType.type()->def(cedar::aux::Enum(Centroid, "Centroid"));
        mType.type()->def(cedar::aux::Enum(Cyclic, "Cyclic Mean"));
      }

      //! @returns A const reference to the base enum object.
      static const cedar::aux::EnumBase& type()
      {
        return *(mType.type());
      }

      //! @returns A pointer to the base enum object.
      static const cedar::proc::DataRole::TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      static const Id Maximum = 0;
      static const Id Centroid = 1;
      static const Id Cyclic = 2;

    private:
      static cedar::aux::EnumType<UnitType> mType;
  };


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PositionOfMaximum();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::DataSlot::VALIDITY determineInputValidity
  (
    cedar::proc::ConstDataSlotPtr slot,
    cedar::aux::ConstDataPtr data
  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void inputConnectionChanged(const std::string& inputName);

  void compute(const cedar::proc::Arguments& arguments);
  void recompute(bool reinit);

private slots:
  void outputTypeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The output data.
  cedar::aux::MatDataPtr mOutput;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::EnumParameterPtr mPositionType;
  // none yet

  cedar::aux::BoolParameterPtr   mNaNIfNoPeak;
  cedar::aux::DoubleParameterPtr mPeakThreshold;
  cedar::aux::DoubleParameterPtr mCentroidThreshold;

}; // class cedar::proc::steps::PositionOfMaximum

#endif // CEDAR_PROC_STEPS_POSITION_OF_MAXIMUM_H

