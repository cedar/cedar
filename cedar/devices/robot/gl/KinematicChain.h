/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChainVisualization.h
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Header for the \em cedar::dev::robot::KinematicChainVisualization class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_VISUALIZATION_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_VISUALIZATION_H

// LOCAL INCLUDES
#include "namespace.h"
#include "KinematicChainModel.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Object.h"

// SYSTEM INCLUDES
#include <cv.h>

/*!@brief Visualization of a kinematic chain
 *
 * This class provides a simple OpenGL visualization of any kinematic chain. It has to be provided with a pointer to an 
 * instance of KinematicChainModel, used to get the transformations to the joint coordinate frames. To actually display
 * the chain, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer for that scene 
 * (cedar::aux::gl::Viewer). 
 */
class cedar::dev::robot::KinematicChainVisualization : public cedar::aux::gl::Object
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChainVisualization(KinematicChainModel* pKinematicChainModel);
  //!@brief destructor
  ~KinematicChainVisualization();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief draws the segment attached to the specified joint
  void drawSegment(const unsigned int index);

  //!@brief draws the end-effector
  void drawEndEffector(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! model of the kinematics, used for calculating transformations to the joint and end-effector frames
  KinematicChainModel* mpKinematicChainModel;
  
}; // class cedar::dev::robot::KinematicChainSimulation

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_VISUALIZATION_H
