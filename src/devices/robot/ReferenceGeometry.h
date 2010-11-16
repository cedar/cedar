/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ReferenceGeometry.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the \em cedar::dev::robot::ReferenceGeometry class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H
#define CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES
#include "src/auxiliaries/ConfigurationInterface.h"
#include "src/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <cv.h>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::ReferenceGeometry : public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // structs
  //--------------------------------------------------------------------------------------------------------------------
protected:
  struct Joint
  {
    std::vector<double> position;
    std::vector<double> axis;
    cedar::aux::math::Limits<double> angleLimits;
    cedar::aux::math::Limits<double> velocityLimits;
  };

  struct LinkSegment
  {
    std::vector<double> centerOfMassPosition;
    std::vector<double> centerOfMassDirection;
    std::vector<double> inertiaMoments;
  };

public:
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::Joint> JointPtr;
  typedef boost::shared_ptr<cedar::dev::robot::ReferenceGeometry::LinkSegment> LinkSegmentPtr;

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  ReferenceGeometry(const std::string& configFileName);

  //!@brief destructor
  virtual ~ReferenceGeometry();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  const cedar::dev::robot::ReferenceGeometry::JointPtr& getJoint(const unsigned int index) const;
  const cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr& getLinkSegment(const unsigned int index) const;
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
  void init();
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
  std::vector<cedar::dev::robot::ReferenceGeometry::JointPtr> _mJoints;
  std::vector<cedar::dev::robot::ReferenceGeometry::LinkSegmentPtr> _mLinkSegments;
  std::vector<double> _mBasePosition;

private:
  // none yet

}; // class cedar::dev:robot::ReferenceGeometry

#endif // CEDAR_DEV_ROBOT_REFERENCE_GEOMETRY_H

