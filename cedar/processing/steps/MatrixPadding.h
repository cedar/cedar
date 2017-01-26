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

    File:        MatrixPadding.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 13

    Description: Header file for the class cedar::proc::steps::MatrixPadding.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_MATRIX_PADDING_H
#define CEDAR_PROC_STEPS_MATRIX_PADDING_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/MatrixPadding.fwd.h"

// SYSTEM INCLUDES


/*!@brief A processing step that pads borders onto an input matrix.
 */
class cedar::proc::steps::MatrixPadding : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief An enum defining the padding mode.
   */
  class PaddingMode
  {
  public:
    //! Type of the enum.
    typedef cedar::aux::EnumId Id;

    //! Pointer to the enumeration type.
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    //! Constructs the enumeration values.
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(PadByBorder, "PadByBorder", "pad by border"));
      mType.type()->def(cedar::aux::Enum(PadToSize, "PadToSize", "pad to size"));
    }

    //! Returns the enum base class.
    static const cedar::aux::EnumBase& type()
    {
      return *mType.type();
    }

    //! Returns a pointer to the enum base class.
    static const cedar::proc::steps::MatrixPadding::PaddingMode::TypePtr& typePtr()
    {
      return mType.type();
    }

    //! The input is padded by the specified amount.
    static const Id PadByBorder = 0;

    //! The output has the given size. The input is padded to match this size.
    static const Id PadToSize = 1;

  private:
    static cedar::aux::EnumType<cedar::proc::steps::MatrixPadding::PaddingMode> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixPadding();

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
  void compute(const cedar::proc::Arguments&);

  void compute2D();

  void compute3D();

  template <typename T> void computeND();

  void inputConnectionChanged(const std::string& inputName);

private slots:
  void updateOutputSize();

  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mInput;

  cedar::aux::MatDataPtr mPadded;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! How the input is padded.
  cedar::aux::EnumParameterPtr _mPaddingMode;

  //! How much border is added in each dimension
  cedar::aux::UIntVectorParameterPtr _mPaddedSize;

  //! Type of border handling.
  cedar::aux::EnumParameterPtr _mBorderType;

}; // class cedar::proc::steps::MatrixPadding

#endif // CEDAR_PROC_STEPS_MATRIX_PADDING_H

