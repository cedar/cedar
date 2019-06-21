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

    File:        StaticGain.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Sum.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/typecheck/SameSizedCollection.h"
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::Sum>
      (
        "Algebra",
        "cedar.processing.steps.Sum"
      )
    );
    declaration->setIconPath(":/steps/sum.svg");
    declaration->setDescription
    (
      "Calculates the sum over all inputs. "
      "All tensor inputs must have the same size."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Sum::Sum()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
  // declare all data
  auto input_slot = this->declareInputCollection("terms");
  cedar::proc::typecheck::SameSizedCollection input_check(/* allow0d = */ true, /* allow1Dtranspositions = */ true);
  cedar::proc::typecheck::SameTypeCollection type_check;
  cedar::proc::typecheck::And sum_check;
  sum_check.addCheck(input_check);
  sum_check.addCheck(type_check);

  input_slot->setCheck(sum_check);

  this->declareOutput("sum", this->mOutput);

  this->mInputs = this->getInputSlot("terms");
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Sum::sumSlot(cedar::proc::ExternalDataPtr slot, cv::Mat& sum, bool lock)
{
  sum.setTo(0.0);
  double scalar_additions = 0.0;

  for (size_t i = 0; i < slot->getDataCount(); ++i)
  {
    auto mat_data = boost::dynamic_pointer_cast<cedar::aux::MatData>(slot->getData(i));
    if (mat_data)
    {
      boost::shared_ptr<QReadLocker> locker;
      if (lock)
      {
        locker = boost::shared_ptr<QReadLocker>(new QReadLocker(&mat_data->getLock()));
      }

      cv::Mat& input_mat = mat_data->getData();

      unsigned int input_dim = cedar::aux::math::getDimensionalityOf(input_mat);
      if (input_dim == 0)
      {
        scalar_additions += cedar::aux::math::getMatrixEntry<double>(input_mat, 0, 0);
      }
      else
      {
        if (!cedar::aux::math::matrixSizesEqual(input_mat, sum))
        {
          if (input_dim == 1)
          {
            sum = cedar::aux::math::canonicalRowVector(0.0 * input_mat.clone());
          }
          else
          {
            sum = 0.0 * input_mat.clone();
          }
        }

        cv::Mat to_add;
        if (input_dim == 1)
        {
          sum += cedar::aux::math::canonicalRowVector(input_mat);
        }
        else
        {
          sum += input_mat;
        }
      }
    }
  }
  if (scalar_additions != 0.0)
  {
    sum += scalar_additions;
  }
}

void cedar::proc::steps::Sum::compute(const cedar::proc::Arguments&)
{
  cedar::proc::steps::Sum::sumSlot(this->mInputs, this->mOutput->getData(), false);
}

void cedar::proc::steps::Sum::inputConnectionChanged(const std::string& /*inputName*/)
{
  if (this->mInputs->getDataCount() > 0)
  {
    // first, check if all inputs are valid
    if (!this->allInputsValid())
    {
      return;
    }

    // make a copy of the output for later determining if the output actually changed
    QWriteLocker r_l(&this->mOutput->getLock());
    cv::Mat old_output = this->mOutput->getData().clone();
    r_l.unlock();

    // then, initialize the output to the appropriate size
    for (size_t i = 0; i < this->mInputs->getDataCount(); ++i)
    {
      auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->mInputs->getData(i));

      if (mat_data)
      {
        // first, make a copy of the input dimensionality (to avoid long locks/possible deadlocks)
        QReadLocker input_l(&mat_data->getLock());
        auto input_dimensionality = mat_data->getDimensionality();
        input_l.unlock();

        // then set the output
        QWriteLocker l(&this->mOutput->getLock());
        if (input_dimensionality == 1)
        {
          this->mOutput->setData(cedar::aux::math::canonicalRowVector(mat_data->getData() * 0.0));
        }
        else
        {
          this->mOutput->setData(mat_data->getData() * 0.0);
        }
        l.unlock();

        // we need to check all data; if one is not 0d, we need to use its size
        if (input_dimensionality > 0)
        {
          break;
        }
      }
    }

    // finally, compute once and then notify subsequent steps that the output size may have changed
    this->callComputeWithoutTriggering();

    cedar::proc::Step::ReadLocker locker(this);
    const cv::Mat& output = this->mOutput->getData();
    bool changed = old_output.type() != output.type() || old_output.size != output.size;
    locker.unlock();

    if (changed)
    {
      this->emitOutputPropertiesChangedSignal("sum");
    }
  }
}
