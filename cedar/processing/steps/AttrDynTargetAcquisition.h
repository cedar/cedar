/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        AttrDynTargetAcquisition.cpp

    Maintainer:  Mathäus Muzalewski
    Email:       mathaeus.muzalewski@ruhr-uni-bochum.de
    Date:        2014 August 28th

    Description: Step for the implementation of an attractor dynamics approach by Hendrik Reimann, ISR 2010 

    Credits:

======================================================================================================================*/

#ifndef ATTR_DYN_TARGET_ACQUISITION_H_
#define ATTR_DYN_TARGET_ACQUISITION_H_

#include <cedar/processing/Step.h> // if we are going to inherit from cedar::proc::Step, we have to include the header
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include "cedar/processing/steps/AttrDynTargetAcquisition.fwd.h"

class cedar::proc::steps::AttrDynTargetAcquisition : public cedar::proc::Step
{
    Q_OBJECT
  public:
    AttrDynTargetAcquisition();
    cedar::proc::DataSlot::VALIDITY determineInputValidity
                                    (
                                      cedar::proc::ConstDataSlotPtr slot,
                                      cedar::aux::ConstDataPtr data
                                    ) const;


  private:

    void compute(const cedar::proc::Arguments&);
    void inputConnectionChanged(const std::string& inputName);


    double mLastTimeStamp;

    // input
    cedar::aux::ConstMatDataPtr mpCurrentVelocity;
    cedar::aux::ConstMatDataPtr mpAngle;
    cedar::aux::ConstMatDataPtr mpOrthogonalInfluence;

protected:
  
    // output
    cedar::aux::MatDataPtr mpVelocityCorrection;
    cedar::aux::MatDataPtr mpTargetAcquisition;

    // params
    cedar::aux::DoubleParameterPtr mpAlphaDir;
    cedar::aux::DoubleParameterPtr mpAlphaVel;
    cedar::aux::DoubleParameterPtr mpSDes;
    cedar::aux::DoubleParameterPtr mpMaxInfluenceAngle;


};

#endif /* ATTR_DYN_TARGET_ACQUISITION_H_ */
