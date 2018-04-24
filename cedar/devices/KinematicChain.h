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

    File:        KinematicChain.h

    Maintainer:  Jean-Stephane Jokeit   
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2010 08 30

    Description: Chain of joints (e.g., a robotic arm).

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KINEMATIC_CHAIN_H
#define CEDAR_DEV_KINEMATIC_CHAIN_H

// CEDAR INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/math/Limits.h"
#include "cedar/devices/lib.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"

#include "cedar/devices/ForwardKinematics.fwd.h"

// FORWARD DECLARATIONS
#include "cedar/devices/KinematicChain.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <map>

/*!@brief Interface for a chain of joints
 *
 * This interface lets you to set angle, velocity and acceleration values for
 * each joint of the kinematic chain. If your hardware (or driver) does not
 * allow you to control velocities or accelerations directly, you can start the
 * the KinematicChain as a thread to handle velocities and accelerations
 * "manually".
 */
class cedar::dev::KinematicChain
:
public cedar::dev::Component
{
  Q_OBJECT

public:
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //TODO: these are needed in more situations and should be a global cedar thing, check where to put them
  //!@brief an enum defining coordinate systems
  enum COORDINATE_SYSTEM
  {
    WORLD_COORDINATES,
    BASE_COORDINATES,
    LOCAL_COORDINATES
  };

  //!@brief describes the hardware properties of a joint.
  struct CEDAR_DEV_LIB_EXPORT Joint : cedar::aux::Configurable
  {
    //! constructor
    Joint();
    //! position of the joint in robot root coordinates
    cedar::aux::DoubleVectorParameterPtr _mpPosition;
    //! axis of rotation of the joint, in robot root coordinates
    cedar::aux::DoubleVectorParameterPtr _mpAxis;
    //! minimum and maximum angle values
    cedar::aux::math::DoubleLimitsParameterPtr _mpAngleLimits;
    //! minimum and maximum velocity values
    cedar::aux::math::DoubleLimitsParameterPtr _mpVelocityLimits;
    //! minimum and maximum acceleration values
    cedar::aux::math::DoubleLimitsParameterPtr _mpAccelerationLimits;
  };

public:
  //! smart pointer definition for the Joint struct
  typedef boost::shared_ptr<cedar::dev::KinematicChain::Joint> JointPtr;
  //!@brief a parameter for a list of joint objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::dev::KinematicChain::Joint> JointListParameter;
  //!@brief a factory that allows allocating joints
  typedef cedar::aux::FactoryManager<cedar::dev::KinematicChain::JointPtr> JointFactoryManager;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(JointListParameter);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // exceptions
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Exception that is thrown when a type that is not installed is requested.
  class InitialConfigurationNotFoundException : public cedar::aux::NotFoundException {};

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChain
  (
    cedar::aux::LocalCoordinateFramePtr pEndEffector
      = cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame())
  );

  virtual ~KinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // Qt slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  void updatedUserSideMeasurementSlot();

public:
  //!@brief updates the geometry to the current configuration of the kinematic chain DONT CALL IT YOURSELF
  CEDAR_DECLARE_DEPRECATED( void updateTransformations() );  // TODO FORWARD KIN

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
   *
   * @param node json node providing the configuration
   */
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  /*!@brief get number of joints in this kinematic chain
   *
   * @return    number of joints
   */
  unsigned int getNumberOfJoints() const;

  /*!@brief Returns a pointer to a specific joint.
   *
   * @return    pointer to joint struct
   * @param index    index of the joint
   */
  const cedar::dev::KinematicChain::JointPtr getJoint(unsigned int index) const;

  /*!@brief get current state of a single joint angle
   *
   * @param index    specifies the joint
   * @return    joint angle value
   */
  float getJointAngle(unsigned int index) const;

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointAnglesMatrix() const);

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  // replaced by getJointAngles()
  CEDAR_DECLARE_DEPRECATED(cv::Mat getCachedJointAngles() const);

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  cv::Mat getJointAngles() const;

  /*!@brief get current state of a single joint velocity
   *
   * @param index    specifies the joint
   * @return    joint velocity value
   */
  float getJointVelocity(unsigned int index) const;

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointVelocitiesMatrix() const);

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  cv::Mat getJointVelocities() const;

  // Replaced by getJointVelocities()
  CEDAR_DECLARE_DEPRECATED(cv::Mat getCachedJointVelocities() const);

  /*!@brief get current state of a single joint acceleration
   *
   * @param index    specifies the joint
   * @return    joint acceleration value
   */
  virtual float getJointAcceleration(unsigned int index) const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointAccelerationsMatrix() const);

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  cv::Mat getJointAccelerations() const;

  // Replaced by getJointAccelerations()
  CEDAR_DECLARE_DEPRECATED(cv::Mat getCachedJointAccelerations() const);

  /*!@brief get current state of measured joint torques, if available
   *
   * @return    vector of joint angles
   */
  cv::Mat getJointTorques() const;

  /*!@brief get current state of measured external joint torques if available
   *
   * @return    vector of joint angles
   */
  cv::Mat getExternalJointTorques() const;

  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  void setJointAngle(unsigned int index, float angle);

  /*!@brief set current state of all joint angles
   *
   * @param angles    Matrix of new joint angle values
   */
  void setJointAngles(const cv::Mat& angles);

  /*!@brief set current state of all joint angles
   *
   * @param angles    vector of new joint angle values
   */
  void setJointAngles(const std::vector<float>& angles);

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
  void setJointVelocity(unsigned int index, float velocity);

  /*!@brief set current state of all joint velocities
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own velocity control and you want to override this
   * method, then you have to return true here. By returning true you indicate
   * to the KinematicChain base class that no integration is necessary.
   *
   * Note that it is sufficient to override setJointVelocity(unsigned int, float).
   *
   * @param velocities    vector of new joint velocity values
   * @return true iff your subclass handles velocity itself
   */
  void setJointVelocities(const cv::Mat& velocities);

  /*!@brief set current state of all joint velocities
   *
   * The KinematicChain base class will always return false here. If your
   * device actually has its own velocity control and you want to override this
   * method, then you have to return true here. By returning true you indicate
   * to the KinematicChain base class that no integration is necessary.
   *
   * Note that it is sufficient to override setJointVelocity(unsigned int, float).
   *
   * @param velocities    vector of new joint velocity values
   * @return true iff your subclass handles velocity itself
   */
  void setJointVelocities(const std::vector<float>& velocities);

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
  void setJointAcceleration(unsigned int index, float acceleration);

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
  void setJointAccelerations(const cv::Mat& accelerations);

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
  void setJointAccelerations(const std::vector<float>& accelerations);

  /*!@brief returns a smart-pointer to the local coordinate frame of the end-effector
   *
   * @return smart-pointer to the end-effector
   */
  cedar::aux::LocalCoordinateFramePtr getEndEffectorCoordinateFrame();

  /*!@brief returns a smart-pointer to the local coordinate frame of the robot root
   *
   * @return smart-pointer to the end-effector
   */
  cedar::aux::LocalCoordinateFramePtr getRootCoordinateFrame();

  /*!@brief sets the end-effector
   *
   * @param pEndEffector new end-effector
   */
  void setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector);

  /*!@brief transformation matrix between base frame and the specified joint frame
   *
   * @param index    index of the joint to which the transformation is given
   * @return    rigid transformation matrix, 4 \f$\times\f$ 4
   */
  cv::Mat getJointTransformation(unsigned int index);

  /*!@brief calculates cartesian Jacobian of a point/vector, in world coordinates
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param result    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   */
  CEDAR_DECLARE_DEPRECATED( void calculateCartesianJacobian
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  ) );

  /*!@brief calculates Cartesian Jacobian of a point/vector
   * slightly slower than calculateJacobian()
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateCartesianJacobian
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  );

  //! \todo explain coordinate system enum and give default
  /*!@brief calculates the temporal derivative of the Cartesian Jacobian of a point/vector
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param result    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   */
  CEDAR_DECLARE_DEPRECATED(void calculateCartesianJacobianTemporalDerivative
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  ) );

  /*!@brief calculates the temporal derivative of the Cartesian Jacobian of a point/vector given in homogeneous
   * coordinates of the relevant joint frame
   *
   * @param point    relevant point for which the Jacobian is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    Jacobian of the given point, in base coordinates, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateCartesianJacobianTemporalDerivative
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  );

  /*!@brief calculates cartesian velocity of a point
   *
   * @param point    point for which the velocity is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    velocity of the given point, in world coordinates (homogeneous) 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateVelocity
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  );

  /*!@brief gives the spatial Jacobian in the current configuration
   *
   * @param index    index of the joint up to which rotations are considered
   * @return    spatial Jacobian, 6 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateSpatialJacobian(unsigned int index);

  /*!@brief gives the temporal derivative of the spatial Jacobian in the current configuration
   *
   * @param index    index of the joint up to which rotations are considered
   * @return    derivative of the spatial Jacobian, 6 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateSpatialJacobianTemporalDerivative(unsigned int index);

  /*!@brief calculates cartesian acceleration of a point
   *
   * @param point    point for which the acceleration is calculated
   * @param jointIndex    index of the joint frame the point is fixed to, joints after that will not move the point
   * @param coordinateFrame    specifies in which coordinate frame the point is represented
   * @return    acceleration of the given point, in world coordinates (homogeneous) 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateAcceleration
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  );

  /*!@brief gives the end-effector position in the current configuration
   *
   * @return    end effector position in homogeneous coordinates, 4 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateEndEffectorPosition();

  /*!@brief gives the transformation from base frame to the end-effector frame in the current configuration
   *
   * @return    rigid transformation matrix from world to root frame, 4 \f$\times\f$ 4 matrix
   */
  cv::Mat getRootTransformation();

  /*!@brief gives the product of exponentials up to the specified joint
   *
   * @return    product of exponentials, 4 \f$\times\f$ 4 matrix
   */
  cv::Mat getProductOfExponentials(unsigned int jointIndex);

  /*!@brief gives the transformation from base frame to the end-effector frame in the current configuration
   *
   * @return    rigid transformation matrix from world to end-effector frame, 4 \f$\times\f$ 4 matrix
   */
  cv::Mat getEndEffectorTransformation();

  /*!@brief gives the cartesian end-effector Jacobian in the current configuration
   *
   * @return    end effector Jacobian, 3 \f$\times\f$ N matrix, where N = number of joints
   */
  cv::Mat calculateEndEffectorJacobian();

  /*!@brief gives the cartesian end-effector velocity
   *
   * @return    end effector velocity, 4 \f$\times\f$ 1 matrix (homogeneous coordinates)
   */
  cv::Mat calculateEndEffectorVelocity();

  /*!@brief gives the cartesian end-effector acceleration
   *
   * @return    end effector acceleration, 4 \f$\times\f$ 1 matrix (homogeneous coordinates)
   */
  cv::Mat calculateEndEffectorAcceleration();

  /*!@brief get joints of a named initial configuration
   *
   * @return    joint values of the initial configuration
   */
  cv::Mat getInitialConfiguration(const std::string& name);

  /*!@brief get the vector of all initial configuration names
   *
   * @return    vector of string: the names of initial configurations
   */
  std::vector<std::string> getInitialConfigurationNames();

  std::string getCurrentInitialConfigurationName();

  /*!@brief get index number of the current initial configuration
   *
   * @return   the index position in the vector of names 
   */
  unsigned int getCurrentInitialConfigurationIndex();

  /*!@brief get joints of the current initial configuration
   *
   * @return    joint values of the initial configuration
   */
  cv::Mat getCurrentInitialConfiguration();

  // See also @applyInitialConfiguration
  bool setCurrentInitialConfiguration(const std::string& s);

  //!@brief add one initial configuration
  void addInitialConfiguration(const std::string& name, const cv::Mat& config);
  //!@brief delete one initial configuration
  void deleteInitialConfiguration(const std::string& name);
  //!@brief check if initial configuration exists
  bool hasInitialConfiguration(const std::string& name);

  //!@brief set the named map of initial configurations
  // prefer using @addInitialConfiguration
  void setInitialConfigurations(std::map<std::string, cv::Mat> configs);
  //!@brief set the currently valid initial configuration and apply it (i.e. move the manipulator to that configuration)
  void applyInitialConfiguration(const std::string& name);

  //!@brief read initial configurations out of chain specific file
  void readInitialConfigurations();

  //!@brief read initial configurations out of a specific node
  void readInitialConfigurations(cedar::aux::ConfigurationNode& initConfigurations);

  //!@brief write initial configurations to a specific node
  void writeInitialConfigurations(cedar::aux::ConfigurationNode& initConfigurations);

  cv::Mat getUpperJointLimits();
  cv::Mat getLowerJointLimits();
  cv::Mat getUpperVelocityLimits();
  cv::Mat getLowerVelocityLimits();
  cv::Mat getUpperAccelerationLimits();
  cv::Mat getLowerAccelerationLimits();

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void init();
  void initializeFromJointList();

  bool applyLimits(const ComponentDataType &type, cv::Mat &data);

  void applyAngleLimits(cv::Mat& angles);
  void applyVelocityLimits(cv::Mat& velocities);
  void applyAccelerationLimits(cv::Mat &accelerations);

  //!@brief set the currently valid initial configuration, do not move the manipulator
  //!@brief: test validity of initial configurations
  void checkInitialConfigurations();

  virtual bool applyBrakeSlowlyController();
  virtual bool applyBrakeNowController();

  virtual void applyInitialConfigurationController(cv::Mat targetConfig);

  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
public:
  static constexpr cedar::dev::Component::ComponentDataType JOINT_ANGLES = 1;
  static constexpr cedar::dev::Component::ComponentDataType JOINT_VELOCITIES = 2;
  static constexpr cedar::dev::Component::ComponentDataType JOINT_ACCELERATIONS = 3;
  static constexpr cedar::dev::Component::ComponentDataType JOINT_TORQUES = 4;
  static constexpr cedar::dev::Component::ComponentDataType EXTERNAL_JOINT_TORQUES = 5;
  static constexpr cedar::dev::Component::ComponentDataType ADDITIONAL_JOINT_TORQUES = 6;

protected:
  //! vector of all joints
  JointListParameterPtr mpJoints;

private:

  //! the forward kinematic model
  std::unique_ptr<ForwardKinematics> mForwardKinematics;

  //!@brief map of the named initial configurations
  std::map<std::string, cv::Mat> mInitialConfigurations;
  //!@brief the current initial configuration name
  std::string mCurrentInitialConfiguration;
  //!@brief lock for the initial configuration datas
  QReadWriteLock mCurrentInitialConfigurationLock;
  //!@brief remember the number of limit excesses per joint, in order to not send them again.
  std::vector<unsigned int> mWarned;

}; // class cedar::dev::robot::KinematicChain

#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_DEV_EXPORT_SINGLETON(cedar::dev::KinematicChain::JointFactoryManager);

namespace cedar
{
  namespace dev
  {
    //!@brief The singleton instance of the joint factory manager.
    typedef cedar::aux::Singleton<cedar::dev::KinematicChain::JointFactoryManager> JointFactoryManagerSingleton;
  }
}
#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_H
