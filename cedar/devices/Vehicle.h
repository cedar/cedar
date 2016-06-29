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
  /*!@brief get number of wheels of this vehicle
   *
   * @return    number of wheels
   */
  unsigned int getNumberOfWheels() const;

  /*!@brief get current state of a single joint velocity
   *
   * @param index    specifies the joint
   * @return    joint velocity value
   */
  double getWheelVelocity(unsigned int index) const;

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  cv::Mat getWheelVelocities() const;

  /*!@brief get current state of a single joint acceleration
   *
   * @param index    specifies the joint
   * @return    joint acceleration value
   */
  virtual double getWheelAcceleration(unsigned int index) const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  cv::Mat getWheelAccelerations() const;

  /*!@brief set current state of a single joint velocity
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own velocity control you would probably like to
   * override this method and return true here. By returning true you indicate
   * to the KinematicChain base class that no integration is necessary.
   *
   * @param index    specifies the joint
   * @param velocity    new joint velocity value
   * @return true iff your subclass handles velocity itself
   */
  void setWheelVelocity(unsigned int index, double velocity);

  /*!@brief set current state of all joint velocities
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own velocity control and you want to override this
   * method, then you have to return true here. By returning true you indicate
   * to the KinematicChain base class that no integration is necessary.
   *
   * Note that it is sufficient to override setJointVelocity(unsigned int, double).
   *
   * @param velocities    vector of new joint velocity values
   * @return true iff your subclass handles velocity itself
   */
  void setWheelVelocities(const cv::Mat& velocities);

  /*!@brief set current state of a single joint acceleration
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own acceleration control you would probably like to
   * override this method and return true here. By returning true you indicate
   * to the KinematicChain base class that no integration is necessary.
   *
   * @param index    specifies the joint
   * @param acceleration    new joint acceleration value
   * @return true iff your subclass handles acceleration itself
   */
  void setWheelAcceleration(unsigned int index, double acceleration);

  /*!@brief set current state of all joint velocities
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own acceleration control and you want to override
   * this method, then you have to return true here. By returning true you
   * indicate to the KinematicChain base class that no integration is necessary.
   *
   * @param accelerations    vector of new joint velocity values
   * @return true iff your subclass handles acceleration itself
   */
  void setWheelAccelerations(const cv::Mat& accelerations);

  /*!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
   *
   * @param node json node providing the configuration
   */
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

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

private:
  // none yet

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
