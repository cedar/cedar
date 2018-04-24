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

    File:        RobotVisualisation.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the head of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOTVIS_H
#define CEDAR_DEV_ROBOTVIS_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/gl/RobotVisualisation.fwd.h"

#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Robot visualisation interface
 *
 * This class serves as an interface between robot visualisations and generic cedar::aux::gl::ObjectVisualization
 */

class cedar::dev::gl::RobotVisualisation : public cedar::aux::gl::ObjectVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief constructor
  RobotVisualisation(cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
                         const std::string& objectType = "no type",
                         double colorR = 1.0,
                         double colorG = 0.0,
                         double colorB = 0.0
                     );

  //!@brief destructor
  ~RobotVisualisation();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief the visualization initializes resources in the current GL context
  virtual void initializeGl() = 0;

  //!@brief draws a visualization of the object in the current GL context
  virtual void draw() = 0;

  //!@brief set method to access the robot pointer
  void setRobotPtr(const cedar::dev::RobotPtr robot)
  {
    mRobot = robot;
  }

private:
  // nothing here

protected:

  //!@brief pointer to a robot, using which component data may be referred (for visualisation purpose)
  cedar::dev::RobotWeakPtr mRobot;

}; // class cedar::dev::gl::RobotVisualisation

#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace dev
  {
    namespace gl
    {
      //!@brief The manager of all sigmoind instances
      typedef cedar::aux::FactoryManager<RobotVisualisationPtr> RobotVisualisationManager;

      //!@brief The singleton object of the TransferFunctionFactory.
      typedef cedar::aux::Singleton<RobotVisualisationManager> RobotVisualisationManagerSingleton;
    }
  }
}

CEDAR_DEV_EXPORT_SINGLETON(cedar::dev::gl::RobotVisualisationManager);


#endif // CEDAR_DEV_ROBOTVIS_H

