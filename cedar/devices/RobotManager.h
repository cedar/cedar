/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        RobotManager.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MANAGER_H
#define CEDAR_DEV_ROBOT_MANAGER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <boost/signals2.hpp>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::RobotManager
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::dev::RobotManager>;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Destructor
  virtual ~RobotManager();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void addRobotName(const std::string& robotName);

  void loadRobotConfigurationFromResource(const std::string& robotName, const std::string& resourcePath);

  cedar::dev::RobotPtr getRobot(const std::string& robotName) const;

  inline boost::signals2::connection connectToRobotNameAddedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mRobotNameAddedSignal.connect(slot);
  }

  inline boost::signals2::connection connectToRobotConfigurationChangedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mRobotConfigurationLoadedSignal.connect(slot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor. Private beacuse this is a singleton object.
  RobotManager();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::map<std::string, cedar::dev::RobotPtr> mRobotInstances;

  std::map<std::string, std::string> mRobotResourceConfigurations;

  boost::signals2::signal<void (const std::string&)> mRobotNameAddedSignal;

  boost::signals2::signal<void (const std::string&)> mRobotConfigurationLoadedSignal;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::RobotManager

#include "cedar/auxiliaries/FactoryManager.h"

namespace cedar
{
  namespace dev
  {
    //!@brief The singleton instance of the robot manager.
    typedef cedar::aux::Singleton<cedar::dev::RobotManager> RobotManagerSingleton;
  }

  namespace aux
  {
    //!@cond SKIPPED_DOCUMENTATION
    CEDAR_INSTANTIATE_AUX_TEMPLATE(cedar::aux::Singleton<cedar::dev::RobotManager>);
    //!@endcond
  }
}

#endif // CEDAR_DEV_ROBOT_MANAGER_H
