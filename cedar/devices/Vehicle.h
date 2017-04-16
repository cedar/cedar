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

 File:        Vehicle.h

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 24

 Description: Header file for the class cedar::dev::Vehicle.

 Credits:

 ======================================================================================================================*/

#ifndef CEDAR_DEV_VEHICLE_H
#define CEDAR_DEV_VEHICLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/devices/Vehicle.fwd.h"

// SYSTEM INCLUDES

/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::Vehicle : public cedar::dev::Component
{
Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

public:
  //!@brief describes the hardware properties of a joint.
  struct CEDAR_DEV_LIB_EXPORT Wheel : cedar::aux::Configurable
  {
    //! constructor
    Wheel();
    //! position of the joint in robot root coordinates
    cedar::aux::DoubleVectorParameterPtr _mpPosition;
    //! minimum and maximum velocity values
    cedar::aux::math::DoubleLimitsParameterPtr _mpVelocityLimits;
  };

  //! smart pointer definition for the Joint struct
  typedef boost::shared_ptr<cedar::dev::Vehicle::Wheel> WheelPtr;
  //!@brief a parameter for a list of joint objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::dev::Vehicle::Wheel> WheelListParameter;
  //!@brief a factory that allows allocating joints
  typedef cedar::aux::FactoryManager<cedar::dev::Vehicle::WheelPtr> WheelFactoryManager;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(WheelListParameter);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Vehicle();

  //!@brief Destructor
  virtual ~Vehicle();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  unsigned int getNumberOfWheels() const;

  float getWheelVelocity(unsigned int index) const;

  cv::Mat getWheelVelocities() const;

  virtual float getWheelAcceleration(unsigned int index) const;

  cv::Mat getWheelAccelerations() const;

  void setWheelVelocity(unsigned int index, float velocity);

  void setWheelVelocities(const cv::Mat& velocities);

  void setWheelAcceleration(unsigned int index, float acceleration);

  void setWheelAccelerations(const cv::Mat& accelerations);

  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  cv::Mat getWheelRotations(float robotRotationChange);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  void initializefromWheelList();

  virtual bool applyBrakeSlowlyController();

  virtual bool applyBrakeNowController();

  float calculateCenterToWheelDistance();

  cv::Mat calculateWheelRotationDirections();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  static const cedar::dev::Component::ComponentDataType WHEEL_VELOCITIES;
  static const cedar::dev::Component::ComponentDataType WHEEL_ACCELERATIONS;
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  WheelListParameterPtr mWheels;
  cedar::aux::DoubleParameterPtr mWheelRadius;
  cv::Mat mWheelRotationDirections;

private:
  float mCenterToWheelDistance; //We assume that wheels are always arranged around a given centerpoint with this distance.

};
// class cedar::dev::Vehicle

#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_DEV_EXPORT_SINGLETON(cedar::dev::Vehicle::WheelFactoryManager);

namespace cedar
{
  namespace dev
  {
    //!@brief The singleton instance of the joint factory manager.
    typedef cedar::aux::Singleton<cedar::dev::Vehicle::WheelFactoryManager> WheelFactoryManagerSingleton;
  }
}


#endif // CEDAR_DEV_VEHICLE_H
