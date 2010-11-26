/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ReferenceGeometry.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the @em cedar::dev::robot::ReferenceGeometry class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H
#define CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/ConfigurationInterface.h"
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <cv.h>


/*!@brief Encapsulates values that describe the physical properties of a robot.
 *
 * Describes the position, axis and angle/velocity limits of all joints of a robot, as well as all properties of
 * all link segments relevant for computing trajectories in a dynamical model.
 */
class cedar::dev::robot::ReferenceGeometry : public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // structs
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Describes the hardware properties of a joint.
  struct Joint
  {
    //! position of a joint in 3D space
    std::vector<double> position;
    //! axis the joint moves in, in 3D space
    std::vector<double> axis;
    //! minimum and maximum angular values
    cedar::aux::math::Limits<double> angleLimits;
    //! minimum and maximum velocity values
    cedar::aux::math::Limits<double> velocityLimits;
  };

  //!@brief Describes the hardware properties of a link segment.
  struct LinkSegment
  {
    //! position of center of mass in 3D space
    std::vector<double> centerOfMassPosition;
    //! orientation of center of mass in 3D space
    std::vector<double> centerOfMassOrientation;
    //! inertia moments in 3D space
    std::vector<double> inertiaMoments;
  };

public:
  //! smart pointer definition for the Joint struct
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::Joint> JointPtr;
  //! smart pointer definition for the LinkSegment struct
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::LinkSegment> LinkSegmentPtr;

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  ReferenceGeometry(const std::string& configFileName);

  //!@brief Destructor
  virtual ~ReferenceGeometry();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns a pointer to a specific joint.
   *
   * @return    pointer to joint struct
   * @param index    index of the joint
   */
  const cedar::dev::robot::ReferenceGeometry::JointPtr& getJoint(const unsigned int index) const;

  /*!@brief Returns a pointer to a specific link segment.
   *
   * @return    pointer to link segment struct
   * @param index    index of the link segment
   */
  const cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr& getLinkSegment(const unsigned int index) const;

  /*!@brief Returns a vector describing the position of the base of the robot in 3D space.
   *
   * @return    base position
   */
  const std::vector<double>& getBasePosition() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Initializes the configuration interface.
   *
   * Since the @em cedar::dev::robot::ReferenceGeometry has a more complex configuration, parts of which the
   * @em cedar::aux::ConfigurationInterface does not support out of the box, the reading process must be prepared.
   */
  void init();

  // TODO: remove when debugged
  void testOutput() const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  //! vector of all joints
  std::vector<cedar::dev::robot::ReferenceGeometry::JointPtr> _mJoints;
  //! vector of all link segments
  std::vector<cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr> _mLinkSegments;
  //! base position of the robot
  std::vector<double> _mBasePosition;

private:
  // none yet

}; // class cedar::dev:robot::ReferenceGeometry

#endif // CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

