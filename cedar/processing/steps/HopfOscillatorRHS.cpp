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

    File:        HopfOscillatorRHS.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2019 11 04

    Description: Source file for the class cedar::proc::steps::HopfOscillatorRHS.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/HopfOscillatorRHS.h"

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
      new ElementDeclarationTemplate< cedar::proc::steps::HopfOscillatorRHS >
      (
        "Numerics",
        "cedar.processing.steps.HopfOscillatorRHS"
      )
    );

    declaration->setIconPath(":/steps/hopf_osci.svg");
    declaration->setDescription
    (
      "Implements the right-hand-side of the Hopf oscillator equation "
      "(the normal form of the Hopf bifurcation)."
      "The oscillator variable is a two dimensional vector, and is "
      "called (u,v) by convention. "
//      "$$\\dot{u} = \\alpha u - \\omega v - \\gamma r^2 u$$\n"
//      "$$\\dot{v} = \\alpha v + \\omega v - \\gamma r^2 v$$\n"
      "Can also be used as a bank of n oscillators. "
      "The state of the bank is a (2xn) matrix, where the columns n are the "
      "size "
      "of the bank."
      "\n"
      "You can set the values for alpha, gamma and omega via parameters. "
      "In that case that single value will apply to all oscillators of the "
      "bank. Alternatively, you can provide an n-dimensional vector "
      "(nx1 matrix) of values of alpha, gamma or omega, one value for each "
      "oscillator in the bank, respectively. Do this via the optional "
      "inputs. This will override the aforementioned global settings for "
      "the banks alpha, gamma, omega."
      "\n"
      "You can shift u by fractions of the limit cycle radius (e.g. -1 "
      "to shift the limit cycle so that it passes through (0,0)) with the "
      "corresponding Parameter."
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

cedar::proc::steps::HopfOscillatorRHS::HopfOscillatorRHS()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(2,1,CV_32F))),
mBankSize(new cedar::aux::UIntParameter(this, "bank size", 1, 1, 1000)),
mGlobalOmega(new cedar::aux::DoubleParameter(this, "bank omega", 1.0)),
mGlobalAlpha(new cedar::aux::DoubleParameter(this, "bank alpha", 1.0)),
mGlobalGamma(new cedar::aux::DoubleParameter(this, "bank gamma", 1.0)),
mInitializeOnReset(new cedar::aux::BoolParameter(this, "initialize on reset", true)),
mShiftUByRadiusFactor(new cedar::aux::DoubleParameter(this, "shift u by radius fraction", 0.0))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("state");
#define INITIAL_STATE_INPUT_NAME "initial state (optional)"
  cedar::proc::DataSlotPtr input2 = this->declareInput(INITIAL_STATE_INPUT_NAME, false); // optional
#define OMEGAS_INPUT_NAME "omegas (bank) (optional)"
  cedar::proc::DataSlotPtr input3 = this->declareInput(OMEGAS_INPUT_NAME, false); // optional
#define ALPHAS_INPUT_NAME "alphas (bank) (optional)"
  cedar::proc::DataSlotPtr input4 = this->declareInput(ALPHAS_INPUT_NAME, false); // optional
#define GAMMAS_INPUT_NAME "gammas (bank) (optional)"
  cedar::proc::DataSlotPtr input5 = this->declareInput(GAMMAS_INPUT_NAME, false); // optional

  this->declareOutput("state change", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  QObject::connect(this->mGlobalOmega.get(), SIGNAL(valueChanged()), this, SLOT(thereIsAChangedSlot()));
  QObject::connect(this->mGlobalAlpha.get(), SIGNAL(valueChanged()), this, SLOT(thereIsAChangedSlot()));
  QObject::connect(this->mGlobalGamma.get(), SIGNAL(valueChanged()), this, SLOT(thereIsAChangedSlot()));
}

void cedar::proc::steps::HopfOscillatorRHS::reinitialize()
{
  auto initialinput= getInput(INITIAL_STATE_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > initialinput_data;
  cv::Mat initialinput_mat;

  if (!initialinput
      || !(initialinput_data= boost::dynamic_pointer_cast<const cedar::aux::MatData>(initialinput) )
      || (initialinput_mat= initialinput_data->getData()).empty() )
  {
    int rows, cols;

    rows= 2;
    cols= mBankSize->getValue();
#if 0
    // if there is an input, resize to its size
    auto input = getInput("state");
    cv::Mat input_mat;
    if (input)
    {
      auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);
      if (data)
      {
        cv::Mat input_mat;
        input_mat = data->getData();
        if (!input_mat.empty())
        {
          //rows= input_mat.rows;
          cols= input_mat.cols;
        }
      }
    }
    else
    {
      // if not assume the bank is 1 wide
      cols= 1;
    }


#endif
    mOutput->setData( cv::Mat::zeros( rows, cols, CV_32F ) );
  }
  else
  {
    mOutput->setData( initialinput_mat.clone() );
  }

  checkOptionalInputs();
}

void cedar::proc::steps::HopfOscillatorRHS::checkOptionalInputs()
{
  auto omegasinput= getInput(OMEGAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > omegas_data;
  cv::Mat omegas_mat;

  if (omegasinput
      && (omegas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(omegasinput) )
      && !(omegas_mat= omegas_data->getData()).empty() )
  {
    mGlobalOmega->setConstant(true);
  }
  else
  {
    mGlobalOmega->setConstant(false);
  }

  auto alphasinput= getInput(ALPHAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > alphas_data;
  cv::Mat alphas_mat;

  if (alphasinput
      && (alphas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(alphasinput) )
      && !(alphas_mat= alphas_data->getData()).empty() )
  {
    mGlobalAlpha->setConstant(true);
  }
  else
  {
    mGlobalAlpha->setConstant(false);
  }

  auto gammasinput= getInput(GAMMAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > gammas_data;
  cv::Mat gammas_mat;

  if (gammasinput
      && (gammas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(gammasinput) )
      && !(gammas_mat= gammas_data->getData()).empty() )
  {
    mGlobalGamma->setConstant(true);
  }
  else
  {
    mGlobalGamma->setConstant(false);
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::HopfOscillatorRHS::determineInputValidity
(
  cedar::proc::ConstDataSlotPtr slot,
  cedar::aux::ConstDataPtr data
)
const
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
    else if (slot->getName() == ALPHAS_INPUT_NAME
             || slot->getName() == OMEGAS_INPUT_NAME
             || slot->getName() == GAMMAS_INPUT_NAME)
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

void cedar::proc::steps::HopfOscillatorRHS::reset()
{
  if (mInitializeOnReset->getValue())
  {
    reinitialize();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::HopfOscillatorRHS::inputConnectionChanged(const std::string&)
{
  reinitialize();
  this->emitOutputPropertiesChangedSignal("state change");
}

void cedar::proc::steps::HopfOscillatorRHS::recompute()
{
  checkOptionalInputs();

  auto input = getInput("state");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data
      || data->isEmpty())
    return;

  cv::Mat input_mat = data->getData();
  cv::Mat out_mat;

  auto my_cols= mBankSize->getValue(); //input_mat.cols;

  if (my_cols > input_mat.cols)
    return;

  out_mat= cv::Mat::zeros(2, my_cols, CV_32F);

  auto omegasinput= getInput(OMEGAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > omegas_data;
  cv::Mat omegas_mat;
  bool use_omegas_bank= false;

  if (omegasinput
      && (omegas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(omegasinput) )
      && !(omegas_mat= omegas_data->getData()).empty() )
  {
    use_omegas_bank= true;
  }

  auto alphasinput= getInput(ALPHAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > alphas_data;
  cv::Mat alphas_mat;
  bool use_alphas_bank= false;

  if (alphasinput
      && (alphas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(alphasinput) )
      && !(alphas_mat= alphas_data->getData()).empty() )
  {
    use_alphas_bank= true;
  }

  auto gammasinput= getInput(GAMMAS_INPUT_NAME);
  boost::shared_ptr< cedar::aux::ConstMatData > gammas_data;
  cv::Mat gammas_mat;
  bool use_gammas_bank= false;

  if (gammasinput
      && (gammas_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(gammasinput) )
      && !(gammas_mat= gammas_data->getData()).empty() )
  {
    use_gammas_bank= true;
  }

  for( unsigned int i=0; i < my_cols; i++ )
  {
    float my_alpha;
    float my_omega;
    float my_gamma;

    if (use_omegas_bank)
    {
      my_omega= omegas_mat.at<float>(i, 0);
    }
    else
    {
      my_omega= mGlobalOmega->getValue();
    }

    if (use_alphas_bank)
    {
      my_alpha= alphas_mat.at<float>(i, 0);
    }
    else
    {
      my_alpha= mGlobalAlpha->getValue();
    }

    if (use_gammas_bank)
    {
      my_gamma= gammas_mat.at<float>(i, 0);
    }
    else
    {
      my_gamma= mGlobalGamma->getValue();
    }

    float my_radius= sqrt( my_alpha / my_gamma );
    float my_u;
    float my_v;

    my_u= input_mat.at<float>(0, i);
    my_u+= mShiftUByRadiusFactor->getValue() * my_radius;

    my_v= input_mat.at<float>(1, i);

    float rsqrd= pow( my_u, 2 ) 
                 + pow( my_v, 2 );

    // u:
    float dotu= my_alpha*my_u - my_omega*my_v - rsqrd*my_gamma*my_u;
    out_mat.at<float>(0,i)= dotu;
    // v:
    float dotv= my_alpha*my_v + my_omega*my_u - rsqrd*my_gamma*my_v;
    out_mat.at<float>(1,i)= dotv;
  }

  this->mOutput->setData(out_mat);
}

void cedar::proc::steps::HopfOscillatorRHS::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::HopfOscillatorRHS::thereIsAChangedSlot()
{
  checkOptionalInputs();
}

void cedar::proc::steps::HopfOscillatorRHS::onStart()
{
  mBankSize->setConstant(true);
}

void cedar::proc::steps::HopfOscillatorRHS::onStop()
{
  mBankSize->setConstant(false);
}

