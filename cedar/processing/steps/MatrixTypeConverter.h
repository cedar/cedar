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

    File:        MatrixTypeConverter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_MATRIX_TYPE_CONVERTER_H
#define CEDAR_PROC_STEPS_MATRIX_TYPE_CONVERTER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/MatrixTypeConverter.fwd.h"

// SYSTEM INCLUDES


/*!@brief A processing step that converts a matrix to a specific type.
 */
class cedar::proc::steps::MatrixTypeConverter : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Enum class for specifying a matrix type.
   */
  class MatrixType
  {
    public:
      //! the id of an enum entry
      typedef cedar::aux::EnumId Id;

      //! constructs the enum for all ids
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(Int8, "Int8"));
        mType.type()->def(cedar::aux::Enum(UInt8, "UInt8"));
        mType.type()->def(cedar::aux::Enum(Float, "Float"));
        mType.type()->def(cedar::aux::Enum(Double, "Double"));
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
      //! id for unsigned byte
      static const Id UInt8 = CV_8U;
      
      //! id for signed byte
      static const Id Int8 = CV_8S;
      
      //! id for floating point
      static const Id Float = CV_32F;
      
      //! id for floating point, double precision
      static const Id Double = CV_64F;

    private:
      static cedar::aux::EnumType<MatrixType> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixTypeConverter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the type to which the matrix should be converted.
   */
  inline MatrixType::Id getTargetType() const
  {
    return this->_mTargetType->getValue();
  }

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

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
private slots:
  void targetTypeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // data
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! The matrix to be converted.
  cedar::aux::ConstMatDataPtr mMatrix;

  //! The converted version of the input matrix.
  cedar::aux::MatDataPtr mConverted;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::EnumParameterPtr _mTargetType;

}; // class cedar::proc::steps::MatrixTypeConverter

#endif // CEDAR_PROC_STEPS_MATRIX_TYPE_CONVERTER_H

