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

    File:        SpaceToRateCode.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/SpaceToRateCode.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

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
      new ElementDeclarationTemplate<cedar::dyn::SpaceToRateCode>
      (
        "DFT",
        "cedar.dynamics.SpaceToRateCode"
      )
    );
    declaration->setIconPath(":/steps/space_to_rate_code.svg");
    declaration->setDescription
    (
      "Transforms space code to rate code. A dynamical system relaxes over time to the center of mass of the "
      "represented metrical interval. The resulting rate code value estimates the center of the distribution if there"
      "is only one maximum and if the distribution is symmetrical."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::SpaceToRateCode::SpaceToRateCode()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
mFixPoint(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
// parameters
_mTau(new cedar::aux::TimeParameter
          (
            this,
            "tau",
            cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::seconds),
            cedar::aux::TimeParameter::LimitType::positive()
          )
     ),
_mLowerLimit(new cedar::aux::DoubleParameter(this, "lowerLimit", 0.0, -10000.0, 10000.0)),
_mUpperLimit(new cedar::aux::DoubleParameter(this, "upperLimit", 1.0, -10000.0, 10000.0))
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);
  this->declareBuffer("fix point", mFixPoint);
  this->mOutput->getData().at<float>(0,0) = 0.0;
  this->limitsChanged();
  // connect the parameter's change signal
  QObject::connect(_mLowerLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  QObject::connect(_mUpperLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*!
 * @class cedar::dyn::SpaceToRateCode
 *
 * This step's update equation contains some mathematical reformulations that warrant explanation.
 *
 * Normally, the Euler step for this particular equation would be calculated according to
 * \f[
 *   x_{t + 1} = x_t + \frac{dt}{\tau} \cdot (-s x_t + o),
 * \f]
 * where
 * \f[
 *   s = \int \sigma(u(x)) dx
 * \f]
 * and
 * \f[
 *   o = \int x \cdot \sigma(u(x)) dx
 * \f]
 * (with \f$\sigma(u(x))\f$ being the input of the step, usually a DNF) and \f$dt\f$ is the time step to be simulated.
 *
 * However, \f$s\f$ and \f$o\f$ can potentially become quite large, leading to instabilities in the numerical
 * approximation of the solution. Thus, our approach to prevent these instabilities is to subdivide \f$dt\f$ into
 * \f$T\f$ substeps, all of length \f$h\f$, except for the last one which is of length \f$h_r\f$, so that
 * \f$dt = T \cdot h + h_r\f$.
 *
 * Given this, we could just calculate the substeps with the usual forward Euler approach. However, this would mean
 * that the Euler step would have to be calculated \f$T + 1\f$ times, which could potentially be a time consuming
 * operation.
 *
 * Instead, we can simplify this by recursively inserting the Euler approximation. First, we reformulate the
 * approximation for the fixed substep \f$h\f$:
 * \f{eqnarray*}{
 *   x_{t + 1} &=& x_{t} + h \cdot \frac{1}{\tau} (-s x_{t} + o) \\
 *             &=& x_{t} - \frac{h}{\tau} s x_{t} + \frac{h}{\tau} o \\
 *             &=& \left( 1 - \frac{h}{\tau} s \right) x_{t} + \frac{h}{\tau} o.
 * \f}
 * Then, we recursively insert this equation into itself:
 * \f{eqnarray*}{
 *  x_{n + 1} &=& \left( 1 - \frac{h}{\tau} s \right) x_{t} + \frac{h}{\tau} o \\
 *          &=& \left( 1 - \frac{h}{\tau} s \right) \left( \left( 1 - \frac{h}{\tau} s \right) x_{t - 1}
 *              + \frac{h}{\tau} o \right) + \frac{h}{\tau} o \\
 *          &=& \left( 1 - \frac{h}{\tau} s\right)^2 x_{t - 1} + \left( 1 - \frac{h}{\tau} s \right) \frac{h}{\tau} o
 *              + \frac{h}{\tau} o \\
 *          &=& \ldots \\
 *          &=& \left( 1 - \frac{h}{\tau} s\right)^{m} x_{t - (m - 1)} + \sum\limits_{i = 0}^{m - 1}
 *              \left( 1 - \frac{h}{\tau} s\right)^i \cdot \frac{h}{\tau} o \\
 *          &=& \ldots \\
 *          &=& \left( 1 - \frac{h}{\tau} s\right)^{t + 1} x_{0} + \sum\limits_{i = 0}^{t}
 *              \left( 1 - \frac{h}{\tau} s\right)^i \cdot \frac{h}{\tau} o
 * \f}
 *
 * The sum in this result can be simplified (see also
 * <a href="http://de.wikipedia.org/wiki/Geometrische_Reihe#Herleitung_der_Formel_f.C3.BCr_die_Partialsummen">this proof
 * on wikipedia</a>). Let \f$v = 1 - \frac{h}{\tau} s\f$. Then
 * \f{eqnarray*}{
 *   s_t &:=& \sum\limits_{i = 0}^{t} v^i \\
 *       &=& 1 + v^1 + v^2 + \ldots + v^t \\
 *   v \cdot s_t &=&  v + v^2 + v^3 + \ldots + v^{t+1} \\
 *   \Rightarrow
 *     s_t - v \cdot s_t &=& 1 - v^{t + 1} \\
 *   \Leftrightarrow s_t (1 - v) &=& 1 - v^{t + 1} \\
 *   \Leftrightarrow s_t &=& \frac{1 - v^{t + 1}}{1 - v}
 * \f}
 * This result only holds for \f$v \neq 1\f$, howerver, for \f$v = 1\f$, calculating the sum is trivial.
 *
 * Inserting this result into the approximation, we get
 * \f{eqnarray*}{
 * x_{t + 1} &=& v^{t + 1} x_{0} + \frac{h}{\tau} o \cdot \sum\limits_{i = 0}^{t} v^i\\
 *    &=&
 *      v^{t + 1} x_{0}
 *      + \frac{h}{\tau} o \cdot \frac{1 - v^{t + 1}}{1 - v}
 * \f}
 *
 * Combined, all these results can be used to arrive at the equation for the complete simulation of our time steps,
 *  \f{eqnarray*}{
 *    x_{T,r} &=& \left( 1 - \frac{h_r}{\tau} s \right) x_T + \frac{h_r}{\tau} o \\
 *     &=& \left( 1 - \frac{h_r}{\tau} s \right)
 *         \left(v^{T + 1} x_{0} + \frac{h}{\tau} o \cdot \frac{1 - v^{T + 1}}{1 - v} \right)
 *         + \frac{h_r}{\tau} o
 *  \f}
 *
 * From the above considerations, we can also derive that this approximation is stable iff
 * \f[
 *   \left| \left( 1 - \frac{h}{\tau} s\right) \right| < 1
 * \f]
 * (because otherwise, the sums do not converge). Because \f$h\f$ and \f$s\f$ are positive, this condition can be
 * rewritten as
 * \f[
 *   \frac{h}{\tau} s < 2,
 * \f]
 * or
 * \f[
 *    h < \frac{2 \tau}{s}.
 * \f]
 */
void cedar::dyn::SpaceToRateCode::eulerStep(const cedar::unit::Time& time)
{
  // the result is simply input * gain; see explanation above for variable names
  double s = cv::sum(this->mInput->getData()).val[0];
  double o = cv::sum(this->mInput->getData().mul(mRamp)).val[0];
  double dt = time / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second);
  //!@todo use the time unit throughout the computation
  double tau = this->getTau() / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second);

  double h = dt;
  if (h / tau * s >= 2) // stability criterion
  {
    h = tau / s; // select a value that is very stable, i.e., is far from the point of instability
  }

  // again, see above for the meaning of the variables
  double T = floor(dt / h);
  double h_r = dt - h * T;
  double v = 1.0 - h * s / tau;
  double v_rest = 1.0 - h_r * s / tau; // defined just like v, just with h_r instead of h
  double x_0 = this->mOutput->getData().at<float>(0,0);
  double v_pow_T = pow(v, T);

  double s_T;
  if (v != 1.0)
  {
    s_T = (1 - v_pow_T) / (1 - v);
  }
  else
  {
    s_T = T;
  }
  this->mOutput->getData().at<float>(0,0) = v_rest * (v_pow_T * x_0 + h * o / tau * s_T) + h_r * o / tau;

  this->mFixPoint->getData().at<float>(0,0) = o / s;
}

void cedar::dyn::SpaceToRateCode::reset()
{
  mOutput->getData() = cv::Scalar(0.0);
}

void cedar::dyn::SpaceToRateCode::limitsChanged()
{
  if (this->mInput && mInput->getDimensionality() == 1 && mInput->getData().type() == CV_32F)
  {
    mRamp = cedar::aux::math::ramp(CV_32F, this->mInput->getData().rows, this->getLowerLimit(), this->getUpperLimit());
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::SpaceToRateCode::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (mat_data->getDimensionality() == 1 && mat_data->getData().type() == CV_32F)
    {
      // Mat data is accepted.
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::SpaceToRateCode::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  if(!this->mInput)
  {
    return;
  }

  if (mInput->getDimensionality() == 1 && mInput->getData().type() == CV_32F)
  {
    this->limitsChanged();
  }
}
