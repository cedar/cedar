/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChainModel.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 08 30

 ----- Description: Header for the \em cedar::dev::robot::KinematicChainModel class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H
#define CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::dev::robot::KinematicChainModel
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KinematicChainModel();
  //!@brief destructor
  virtual ~KinematicChainModel(void) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void getEndEffectorPosition(void) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  cedar::dev::robot::KinematicChainPtr mpChain;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::dev::robot::Component

#endif // CEDAR_DEV_ROBOT_KINEMATIC_CHAIN_MODEL_H

