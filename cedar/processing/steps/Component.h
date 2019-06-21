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

    File:        Component.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_COMPONENT_H
#define CEDAR_PROC_STEPS_COMPONENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/devices/ComponentParameter.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/Component.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>


namespace cedar
{
  namespace proc
  {
    namespace details
    {
      //!@todo This mixes GUI and step -- is this ok?
      class ComponentStepUserSelectableCommandTypeSubsetParameterWidget : public cedar::aux::gui::Parameter
      {
        Q_OBJECT

      public:
        ComponentStepUserSelectableCommandTypeSubsetParameterWidget();

        void parameterChanged();

      private:
        void applyProperties();

        void propertiesChanged();

        void rebuildUserSelectableCommandTypeSubsetList();

      private slots:
        void componentChanged();
        void selectedUserSelectableCommandTypeSubsetChanged(const QString& group);

      private:
        QComboBox* mpSelector;
      };

      class ComponentStepUserSelectableCommandTypeSubsetParameter : public cedar::aux::ParameterTemplate<std::string>
      {
        Q_OBJECT

      public:
        ComponentStepUserSelectableCommandTypeSubsetParameter(cedar::aux::Configurable* owner, const std::string& name);

        void setComponent(cedar::dev::ComponentPtr component);

        cedar::dev::ComponentPtr getComponent();
        bool hasComponent();

      signals:
        void componentChanged();

      private:
        boost::weak_ptr<cedar::dev::Component> mWeakComponent;
      };
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ComponentStepUserSelectableCommandTypeSubsetParameter);
    }
  }
}


/*!@brief A step for bringing device components into a processing architecture.
 */
class cedar::proc::steps::Component : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Component();

  virtual ~Component();

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

signals:
  void componentChanged();

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

  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  void onStart();

  void onStop();

  void rebuildInputs();

  void rebuildOutputs();

  void reset();

  virtual void inputConnectionChanged(const std::string& inputName);

  void brakeSlowly();
  void brakeHard();
  void connectManually();
  void disconnectManually();
  void openRobotManager();

  void applyCurrentInitialConfiguration();

  void readConfiguration(const cedar::aux::ConfigurationNode& node) override;

private slots:
  void componentChangedSlot();

  void selectedUserSelectableCommandTypeSubsetChanged();

  void communicationStepSizeChanged();

  void testStates(cedar::dev::ComponentPtr component);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Id used for signaling "step measurement" times
  unsigned int mMeasurementTimeId;

  //! Id used for signaling "step command" times
  unsigned int mCommandTimeId;

  std::map<std::string, cedar::aux::MatDataPtr>  mOutputs;

  bool mInitConfigHasBeenApplied;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::ComponentParameterPtr _mComponent;

  cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterPtr _mUserSelectableCommandTypeSubset;

  //! Type of the channel stored in this component.
  cedar::aux::DoubleParameterPtr _mCommunicationStepSize;

  cedar::aux::BoolParameterPtr _mUseKinChainConfigurationOnReset;

}; // class cedar::proc::steps::Component

#endif // CEDAR_PROC_STEPS_COMPONENT_H

