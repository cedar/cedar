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

    File:        Transpose.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 14

    Description: Source file for the class cedar::proc::steps::Transpose.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Transpose.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Transpose>
      (
        "Algebra",
        "cedar.processing.Transpose"
      )
    );
    declaration->setIconPath(":/steps/matrix_transpose.svg");
    declaration->setDescription
    (
      "Transposes matrices."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// private nested classes
//----------------------------------------------------------------------------------------------------------------------
/*!@brief An output relation for transposed matrices, which switches rows and columns.
 */
class cedar::proc::steps::Transpose::TransposeMatrixProperties : public cedar::proc::OutputRelation<cedar::aux::MatData>
{
public:
  TransposeMatrixProperties(cedar::proc::DataSlotPtr target)
  :
  cedar::proc::OutputRelation<cedar::aux::MatData>(target)
  {
  }

protected:
  bool assign(cedar::aux::ConstMatDataPtr source, cedar::aux::DataPtr target)
  {
    auto mat_target = boost::dynamic_pointer_cast<cedar::aux::MatData>(target);
    CEDAR_DEBUG_ASSERT(mat_target);

    const cv::Mat& target_mat = mat_target->getData();

    cv::Mat clone;
    if (source)
    {
      const auto& source_mat = source->getData();
      int type = source_mat.type();

      clone = cv::Mat(source_mat.cols, source_mat.rows, type, cv::Scalar(0));
    }

    bool changed = false;
    if (target_mat.type() != clone.type() || target_mat.size != clone.size)
    {
      changed = true;
    }

    mat_target->setData(clone);

    return changed;
  }
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Transpose::Transpose()
:
mInput(this, "matrix"),
mTransposed(new cedar::aux::MatData(cv::Mat()))
{
  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(0, 2);
  mInput.getSlot()->setCheck(input_check);

  auto output = this->declareOutput("transposed", mTransposed);
  mInput.addOutputRelation(TransposeMatrixPropertiesPtr(new TransposeMatrixProperties(output)));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Transpose::compute(const cedar::proc::Arguments&)
{
  this->mTransposed->setData(this->mInput.getData().t());
}
