/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

 File:        InverseKinematicsStep.h

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 29

 Description: Header file for the class cedar::proc::steps::InverseKinematicsStep.

 Credits:

 ======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_INVERSE_KINEMATICS_STEP_H
#define CEDAR_PROC_STEPS_INVERSE_KINEMATICS_STEP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/devices/ComponentParameter.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/PseudoInverseKinematics.fwd.h"

// SYSTEM INCLUDES

/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::PseudoInverseKinematics : public cedar::proc::Step
{
Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PseudoInverseKinematics();

  //!@brief Destructor
  virtual ~PseudoInverseKinematics();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! get the component
  inline cedar::dev::ComponentPtr getComponent() const
  {
    return this->_mComponent->getValue();
  }

  bool hasComponent() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::proc::DataSlot::VALIDITY determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const;
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);

  void onStart();

  void onStop();

  void reset();

private slots:
  void rebuildOutputs();

  void testStates(cedar::dev::ComponentPtr component);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::MatDataPtr mOutputVelocity;
  std::string mInputVelocityName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::ComponentParameterPtr _mComponent;

};
// class cedar::proc::steps::InverseKinematicsStep

#endif // CEDAR_PROC_STEPS_INVERSE_KINEMATICS_STEP_H
