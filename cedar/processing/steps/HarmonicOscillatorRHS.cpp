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

    File:        HarmonicOscillatorRHS.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2019 11 04

    Description: Source file for the class cedar::proc::steps::HarmonicOscillatorRHS.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/HarmonicOscillatorRHS.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
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
      new ElementDeclarationTemplate< cedar::proc::steps::HarmonicOscillatorRHS >
      (
        "Numerics",
        "cedar.processing.steps.HarmonicOscillatorRHS"
      )
    );

    //declaration->setIconPath(":/steps/hopf_osci.svg");
    declaration->setDescription
    (
      "Implements the right-hand-side of a damped harmonic oscillator equation "
      "Can also be used as a bank of n oscillators. "
      "The state of the bank is a (2xn) matrix, where the columns n are the "
      "size "
      "of the bank."
      "\n"
      "You can set the values for damping and stiffness via parameters. "
      "In that case that single value will apply to all oscillators of the "
      "bank. Alternatively, you can provide an n-dimensional vector "
      "(nx1 matrix) of values of damping or stiffness, one value for each "
      "oscillator in the bank, respectively. Do this via the optional "
      "inputs. This will override the aforementioned global settings for "
      "the banks damping and stiffness."
      "\n"
      "There is a parameter that forces the system to be critically damped "
      "and will override all other values for damping."
      "\n"
      "Note: Use the NumericIntegration step to integrate the output "
      "('state change')"
      "and connect the new state back in as the input ('state')."
      "\n"
      "Note: Is intentionally not a looped step since it only implements "
      "the RHS of the differential equation - but keep in mind that "
      "NumericIntegration is looped."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::HarmonicOscillatorRHS::HarmonicOscillatorRHS()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1,1,CV_32F))),
mBankSize(new cedar::aux::UIntParameter(this, "bank size", 1, 1, 1000)),
mGlobalStiffness(new cedar::aux::DoubleParameter(this, "bank stiffness", 1.0)),
mGlobalDamping(new cedar::aux::DoubleParameter(this, "bank damping", 1.0)),
mCriticallyDamped(new cedar::aux::BoolParameter(this, "force critically damped", false))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("state");
  cedar::proc::DataSlotPtr input1b = this->declareInput("shift (optional)", false); // optional

#define INITIAL_STATE_INPUT_NAME "initial state (optional)"
  cedar::proc::DataSlotPtr input2 = this->declareInput(INITIAL_STATE_INPUT_NAME, false); // optional
#define STIFFNESS_INPUT_NAME "stiffnesss (bank) (optional)"
  cedar::proc::DataSlotPtr input3 = this->declareInput(STIFFNESS_INPUT_NAME, false); // optional
#define DAMPINGS_INPUT_NAME "damping (bank) (optional)"
  cedar::proc::DataSlotPtr input4 = this->declareInput(DAMPINGS_INPUT_NAME, false); // optional

  this->declareOutput("state change", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  QObject::connect(this->mGlobalStiffness.get(), SIGNAL(valueChanged()), this, SLOT(thereIsAChangedSlot()));
  QObject::connect(this->mGlobalDamping.get(), SIGNAL(valueChanged()), this, SLOT(thereIsAChangedSlot()));
}

void cedar::proc::steps::HarmonicOscillatorRHS::reinitialize()
{
  int rows, cols;

  rows= 2;
  cols= mBankSize->getValue();
  mOutput->setData( cv::Mat::zeros( rows, cols, CV_32F ) );

  checkOptionalInputs();
}

void cedar::proc::steps::HarmonicOscillatorRHS::checkOptionalInputs()
{
  bool force_critically= mCriticallyDamped->getValue();

  auto stiffnesssinput= getInput(STIFFNESS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > stiffnesss_data;
  cv::Mat stiffnesss_mat;

  if (stiffnesssinput
      && (stiffnesss_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(stiffnesssinput) )
      && !(stiffnesss_mat= stiffnesss_data->getData()).empty() )
  {
    mGlobalStiffness->setConstant(true);
  }
  else
  {
    mGlobalStiffness->setConstant(false);
  }

  auto dampingsinput= getInput(DAMPINGS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > dampings_data;
  cv::Mat dampings_mat;

  if (force_critically)
  {
    mGlobalDamping->setConstant(true);
  }
  else
  {
    if (dampingsinput
        && (dampings_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(dampingsinput) )
        && !(dampings_mat= dampings_data->getData()).empty() )
    {
      mGlobalDamping->setConstant(true);
    }
    else
    {
      mGlobalDamping->setConstant(false);
    }
  }

}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::HarmonicOscillatorRHS::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data);
  if (input && !input->getData().empty())
  {
    const unsigned cols = input->getData().cols;
    const unsigned current_rows = static_cast<unsigned int>(input->getData().rows);
    const unsigned bank_size = mBankSize->getValue();

    if (slot->getName() == "state"
        || slot->getName() == INITIAL_STATE_INPUT_NAME) 
    {
      //input must be a one dimensional vector
      if (cols == bank_size && current_rows == 2)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else if (slot->getName() == DAMPINGS_INPUT_NAME
             || slot->getName() == STIFFNESS_INPUT_NAME
             || slot->getName() == "shift (optional)")
    {
      if (current_rows == bank_size
          && cols == 1)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::HarmonicOscillatorRHS::reset()
{
  reinitialize();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::HarmonicOscillatorRHS::inputConnectionChanged(const std::string&)
{
  reinitialize();
  this->emitOutputPropertiesChangedSignal("state change");
}

void cedar::proc::steps::HarmonicOscillatorRHS::recompute()
{
  checkOptionalInputs();
  bool force_critically= mCriticallyDamped->getValue();

  auto input = getInput("state");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data
      || data->isEmpty())
    return;

  cv::Mat input_mat = data->getData();
  cv::Mat out_mat;

  int my_cols= mBankSize->getValue(); //input_mat.cols;

  if (my_cols > input_mat.cols)
    return;


  cv::Mat shift_mat;
  bool has_shift= false;
  auto shift_input = getInput("shift (optional)");
  if (shift_input)
  {
    auto shift_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(shift_input);

    if (shift_data 
        && !shift_data->isEmpty())
    {
      shift_mat= shift_data->getData();
      has_shift= true;
    }
  }

  out_mat= cv::Mat::zeros(2, my_cols, CV_32F);

  auto stiffnesssinput= getInput(STIFFNESS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > stiffnesss_data;
  cv::Mat stiffnesss_mat;
  bool use_stiffnesss_bank= false;

  if (stiffnesssinput
      && (stiffnesss_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(stiffnesssinput) )
      && !(stiffnesss_mat= stiffnesss_data->getData()).empty() )
  {
    use_stiffnesss_bank= true;
  }

  auto dampingsinput= getInput(DAMPINGS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > dampings_data;
  cv::Mat dampings_mat;
  bool use_dampings_bank= false;

  if (dampingsinput
      && (dampings_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(dampingsinput) )
      && !(dampings_mat= dampings_data->getData()).empty() )
  {
    use_dampings_bank= true;
  }




  for( unsigned int i=0; i < my_cols; i++ )
  {
    float my_damping;
    float my_stiffness;

    if (use_stiffnesss_bank)
    {
      my_stiffness= stiffnesss_mat.at<float>(i, 0);
    }
    else
    {
      my_stiffness= mGlobalStiffness->getValue();
    }

    if (force_critically)
    {
      my_damping= 2 * sqrt( my_stiffness );
    }
    else
    {
      if (use_dampings_bank)
      {
        my_damping= dampings_mat.at<float>(i, 0);
      }
      else
      {
        my_damping= mGlobalDamping->getValue();
      }
    }


    // the equation ...

    float my_pos= input_mat.at<float>(0, i);
    float my_vel= input_mat.at<float>(1, i);

    float dot_pos= my_vel;
    out_mat.at<float>(0,i)= dot_pos;

    float my_shift;
    if (has_shift)
    {
      my_shift= shift_mat.at<float>(0,i);
    }
    else
    {
      my_shift= 0.0;
    }

    float dot_vel= - my_damping * my_vel - my_stiffness * ( my_pos - my_shift );
    out_mat.at<float>(1,i)= dot_vel;
  }

  this->mOutput->setData(out_mat);
}

void cedar::proc::steps::HarmonicOscillatorRHS::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::HarmonicOscillatorRHS::thereIsAChangedSlot()
{
  checkOptionalInputs();
}

void cedar::proc::steps::HarmonicOscillatorRHS::thereIsAChangedParameter()
{
  checkOptionalInputs();
}

void cedar::proc::steps::HarmonicOscillatorRHS::onStart()
{
  mBankSize->setConstant(true);
}

void cedar::proc::steps::HarmonicOscillatorRHS::onStop()
{
  mBankSize->setConstant(false);
}

