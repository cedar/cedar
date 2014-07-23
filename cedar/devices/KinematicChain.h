/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
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
#include "cedar/devices/namespace.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"

// FORWARD DECLARATIONS

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
public cedar::dev::Component,
public cedar::aux::LoopedThread
{
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

  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------
public:
  //!@brief The different modes to operate the kinematic chain
  enum ActionType { ANGLE, VELOCITY, ACCELERATION, STOP };
               // TODO: jokeit:  ANGLE --> unthreaded mode

protected:
  // none yet
private:
  // none yet

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
  //!@brief destructor
  virtual ~KinematicChain();

  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief implementation of handling routine for Qt timer events
   *
   * @param    event
   */
  void timerEvent(QTimerEvent* event);

  //--------------------------------------------------------------------------------------------------------------------
  // Qt slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //!@brief updates the geometry to the current configuration of the kinematic chain
  void updateTransformations();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
public:
  /*!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
   *
   * @param node json node providing the configuration
   */
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  /*!@brief check whether the kinematic chain is currently responsive to movement commands
   *
   * @return    state
   */
  virtual bool isMovable() const = 0;

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
  virtual double getJointAngle(unsigned int index) const = 0;

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  CEDAR_DECLARE_DEPRECATED(std::vector<double> getJointAngles() const);

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointAnglesMatrix() const);

  /*!@brief get current state of all joint angles
   *
   * @return    vector of joint angles
   */
  cv::Mat getCachedJointAngles() const;

  /*!@brief get current state of a single joint velocity
   *
   * @param index    specifies the joint
   * @return    joint velocity value
   */
  virtual double getJointVelocity(unsigned int index) const;

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  CEDAR_DECLARE_DEPRECATED(std::vector<double> getJointVelocities() const);

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointVelocitiesMatrix() const);

  /*!@brief get current state of all joint velocities
   *
   * @return    vector of joint velocities
   */
  cv::Mat getCachedJointVelocities() const;

  /*!@brief get current state of a single joint acceleration
   *
   * @param index    specifies the joint
   * @return    joint acceleration value
   */
  virtual double getJointAcceleration(unsigned int index) const;

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  CEDAR_DECLARE_DEPRECATED(std::vector<double> getJointAccelerations() const);

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  CEDAR_DECLARE_DEPRECATED(cv::Mat getJointAccelerationsMatrix() const);

  /*!@brief get current state of all joint accelerations
   *
   * @return    vector of joint accelerations
   */
  cv::Mat getCachedJointAccelerations() const;

  /*!@brief returns the mode in which the joints positions are set (angle/velocity/acceleration)
   *
   * @return current working mode
   */
  CEDAR_DECLARE_DEPRECATED(ActionType getWorkingMode());

  /*!@brief set current state of a single joint angle
   *
   * @param index    specifies the joint
   * @param angle    new joint angle value
   */
  virtual void setJointAngle(unsigned int index, double angle) = 0;

  /*!@brief set current state of all joint angles
   *
   * @param angles    Matrix of new joint angle values
   */
  void setJointAngles(const cv::Mat& angles);

  /*!@brief set current state of all joint angles
   *
   * @param angles    vector of new joint angle values
   */
  void setJointAngles(const std::vector<double>& angles);

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
  virtual bool setJointVelocity(unsigned int index, double velocity);

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
  bool setJointVelocities(const cv::Mat& velocities);

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
  bool setJointVelocities(const std::vector<double>& velocities);

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
  virtual bool setJointAcceleration(unsigned int index, double acceleration);

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
  bool setJointAccelerations(const cv::Mat& accelerations);

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
  bool setJointAccelerations(const std::vector<double>& accelerations);

  /*!@brief Sets the mode in which the joints positions are set (angle/velocity/acceleration)
   *
   * Setting a working mode will also stop the KinematicChain to allow you to
   * set new target values for each joint. You have to restart the
   * KinematicChain afterwards if you want it to take care of velocities or
   * accelerations. If your hardware lets you set these values directly you
   * do not want to start the thread of KinematicChain.
   *
   * @param actionType new working mode
   */
  CEDAR_DECLARE_DEPRECATED(virtual void setWorkingMode(ActionType actionType));

  /*!@brief Controls if real hardware values are used when integrating velocity/acceleration.
   *
   * For hardware that does not support velocity or acceleration control you can
   * start the KinematicChain as a thread to simulate this behavior. For such a
   * simulation a vector of ideal angle/velocity values is kept. But if you
   * trust your hardware to be fast and reliable enough, you can also integrate
   * using the current hardware values.
   *
   * @param useCurrentHardwareValues
   */
  void useCurrentHardwareValues(bool useCurrentHardwareValues);

#if 0
// TODO: remove
  /*!@brief Starts the kinematic chain as a thread
   *
   * If you want to use velocity or acceleration control but your hardware
   * does not support this, start the thread to "simulate" these values.
   */
  virtual void start(Priority priority = InheritPriority);
#endif  

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
  void calculateCartesianJacobian
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  );

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
  void calculateCartesianJacobianTemporalDerivative
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  );

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
  cv::Mat getInitialConfiguration(std::string name);

  /*!@brief get the vector of all initial configuration names
   *
   * @return    vector of string: the names of initial configurations
   */
  std::vector<std::string> getInitialConfigurationIndices();

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

  //!@brief add one initial configuration
  void addInitialConfiguration(const std::string &name, const cv::Mat &config);
  //!@brief delete one initial configuration
  void deleteInitialConfiguration(const std::string &name);
  //!@brief set the named map of initial configurations
  //
  // prefer using @addInitialConfiguration
  void setInitialConfigurations(std::map<std::string, cv::Mat> configs);
  //!@brief set the currently valid initial configuration and apply it (i.e. move the manipulator to that configuration)
  bool applyInitialConfiguration(std::string s);
  //!@brief apply the named initial configuration by index
  //
  // Prefer using @applyInitialConfiguration(string) for accessing named configurations
  bool applyInitialConfiguration(unsigned int i);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------
protected:

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------
private:
  void step(cedar::unit::Time time);
  void init();
  void initializeFromJointList();
  void applyAngleLimits(cv::Mat& angles);
  void applyVelocityLimits(cv::Mat& velocities);

  //!@brief calculates the transformations to the joint frames for the given joint angle vector
  void calculateTransformations();

  /*!@brief gives the temporal derivative of a joint twist in the current configuration
   *
   * @param jointIndex index of the joint twist
   * @return derivative of the joint twist, 6 \f$\times\f$ 1 matrix
   */
  cv::Mat calculateTwistTemporalDerivative(unsigned int jointIndex);

  //!@brief set the currently valid initial configuration, do not move the manipulator
  // 
  // See also @applyInitialConfiguration
  bool setCurrentInitialConfiguration(const std::string &s);

  //!@brief: test validity of initial configurations
  void checkInitialConfigurations();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  //!@brief current state of the joint angles, see also mAnglesLock
  cv::Mat mJointAngles;
  //!@brief the lock of the mJointAngles
  mutable QReadWriteLock mAnglesLock;

private:
  bool mUseCurrentHardwareValues;
  cv::Mat mJointVelocities;
  mutable QReadWriteLock mVelocitiesLock;
  cv::Mat mJointAccelerations;
  mutable QReadWriteLock mAccelerationsLock;
  ActionType mWorkingMode; // TODO: remove
  mutable QReadWriteLock mWorkingModeLock; // TODO: remove

  //! vector of all joints
  JointListParameterPtr mpJoints;

  //!@brief pointer to the root coordinate frame of the kinematic chain
  cedar::aux::LocalCoordinateFramePtr mpRootCoordinateFrame;
  //!@brief pointer to the local coordinate frame of the end-effector
  cedar::aux::LocalCoordinateFramePtr mpEndEffectorCoordinateFrame;

  // locking for thread safety
  QReadWriteLock mTransformationsLock;
  //! twist coordinates for the transformations induced by rotating the joints (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTwists;
  //! transformations to the joint frames (assuming reference configurations)
  std::vector<cv::Mat> mReferenceJointTransformations;
  //! transformations to the end-effector frame (assuming reference configurations)
  cv::Mat mReferenceEndEffectorTransformation;
    //! exponentials of joint twists with specified joint angle
  std::vector<cv::Mat> mTwistExponentials;
  //! transformation matrices between joints, generated by exponential map of joint twists
  std::vector<cv::Mat> mProductsOfExponentials;
  //! transformation matrices to the joint frames in the current configuration
  std::vector<cv::Mat> mJointTransformations;
  //! twist coordinates for the transformations induced by rotating the joints in the curent configuration
  std::vector<cv::Mat> mJointTwists;

  //!@brief map of the named initial configurations
  std::map< std::string, cv::Mat > mInitialConfigurations;
  //!@brief the current initial configuration name
  std::string mCurrentInitialConfiguration;
  //!@brief lock for the initial configuration datas
  QReadWriteLock mCurrentInitialConfigurationLock;
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

