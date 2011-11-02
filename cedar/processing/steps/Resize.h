/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Resize.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 02

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_RESIZE_H
#define CEDAR_PROC_STEPS_RESIZE_H

// LOCAL INCLUDES
#include "cedar/processing/steps/namespace.h"

// PROJECT INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DataTemplate.h"

// SYSTEM INCLUDES


/*!@brief   A processing step that resizes a matrix.
 */
class cedar::proc::steps::Resize : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  class Interpolation
  {
    public:
      typedef cedar::aux::EnumId Id;
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(LINEAR, "LINEAR", "Linear"));
        mType.type()->def(cedar::aux::Enum(NEAREST, "NEAREST", "Nearest"));
        mType.type()->def(cedar::aux::Enum(AREA, "AREA", "Area"));
        mType.type()->def(cedar::aux::Enum(CUBIC, "CUBIC", "Cubic"));
        mType.type()->def(cedar::aux::Enum(LANCZOS4, "LANCZOS4", "Lanczos 4 (8 x 8)"));
      }

      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      static const TypePtr& typePtr()
      {
        return mType.type();
      }

      static const Id LINEAR = cv::INTER_LINEAR;
      static const Id NEAREST = cv::INTER_NEAREST;
      static const Id AREA = cv::INTER_AREA;
      static const Id CUBIC = cv::INTER_CUBIC;
      static const Id LANCZOS4 = cv::INTER_LANCZOS4;

    private:
      static cedar::aux::EnumType<Interpolation> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Resize();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);


public slots:
  void outputSizeChanged();
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::DataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  cv::Size getOutputSize() const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::MatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The factor by which the input is multiplied.
  cedar::aux::UIntVectorParameterPtr _mOutputSize;
  cedar::aux::EnumParameterPtr _mInterpolationType;

private:

}; // class cedar::proc::steps::Resize

#endif // CEDAR_PROC_STEPS_RESIZE_H

