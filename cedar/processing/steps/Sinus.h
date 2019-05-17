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

    File:        Sinus.h

    Maintainer:  Nico Kuerschner
    Email:       nico.kuerschner@ini.rub.de
    Date:        2016 08 12

    Description: 

    Credits:

======================================================================================================================*/

#ifndef SINUS_DYNAMICS_H_
#define SINUS_DYNAMICS_H_

#include <cedar/processing/Step.h> // if we are going to inherit from cedar::proc::Step, we have to include the header
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include "cedar/processing/steps/Sinus.fwd.h"

class cedar::proc::steps::Sinus : public cedar::proc::Step
{
    Q_OBJECT

public slots:
  void constantChanged();

  public:
    Sinus();
    cedar::proc::DataSlot::VALIDITY determineInputValidity
                                    (
                                      cedar::proc::ConstDataSlotPtr slot,
                                      cedar::aux::ConstDataPtr data
                                    ) const;

  private:
    void recompute();
    void compute(const cedar::proc::Arguments&);
    void inputConnectionChanged(const std::string& inputName);
    void checkOptionalInputs();

  protected:
    // output
    cedar::aux::MatDataPtr mResult;

    // params
    cedar::aux::DoubleParameterPtr mAmplitude;
    cedar::aux::DoubleParameterPtr mShift;
    cedar::aux::BoolParameterPtr mMakeCos;

};

#endif /* SINUS_DYNAMICS_H_ */
