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

    File:        UnitConverter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_UNIT_CONVERTER_H
#define CEDAR_PROC_STEPS_UNIT_CONVERTER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/UnitConverter.fwd.h"

// SYSTEM INCLUDES


/*!@brief Step that converts a scalar value to a value with a unit.
 */
class cedar::proc::steps::UnitConverter : public cedar::proc::Step
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
        mType.type()->def(cedar::aux::Enum(Velocity, "Velocity"));
        mType.type()->def(cedar::aux::Enum(AngularVelocity, "AngularVelocity"));
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
      static const Id Velocity = 0;
      static const Id AngularVelocity = 1;

    private:
      static cedar::aux::EnumType<UnitType> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UnitConverter();

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
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  void inputConnectionChanged(const std::string& inputName);

  void compute(const cedar::proc::Arguments&);

private slots:
  void outputTypeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mInput;

  cedar::aux::DataPtr mOutput;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Factor by which the value is multiplied before conversion. Useful if the input is not a unit type, yet.
  cedar::aux::DoubleParameterPtr _mConversionFactor;

  cedar::aux::EnumParameterPtr _mTargetType;

}; // class cedar::proc::steps::UnitConverter

#endif // CEDAR_PROC_STEPS_UNIT_CONVERTER_H

