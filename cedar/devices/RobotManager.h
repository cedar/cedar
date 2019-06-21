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
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"
#include "cedar/devices/gl/RobotVisualisation.h"

// FORWARD DECLARATION
#include "cedar/devices/RobotManager.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN
#include <map>
#include <vector>
#include <string>


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
public:
  class Template
  {
    public:
      void addNamedConfiguration(const std::string& name, const cedar::aux::Path& resource);

      std::vector<std::string> getConfigurationNames() const;


      inline void setIconPath(const std::string& iconPath)
      {
        this->mIconPath = iconPath;
      }

      inline const std::string& getIconPath() const
      {
        return this->mIconPath;
      }

      cedar::aux::Path getConfiguration(const std::string& name) const;
      bool hasConfiguration(const std::string& name) const;

    private:
      std::map<std::string, cedar::aux::Path> mNamedPaths;

      std::string mIconPath;
  };

private:
  struct RobotInfo
  {
    std::string mTemplateName;
    std::string mLoadedTemplateConfigurationName;
    std::string mLoadedTemplateConfiguration;
    bool mDoesAutomaticallyConnect;
  };

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

  void removeRobot(const std::string& robotName);

  void renameRobot(const std::string& robotName, const std::string& newName);

  void loadRobotConfiguration(const std::string& robotName, const cedar::aux::Path& configuration);

  void loadRobotTemplateConfiguration(const std::string& robotName, const std::string& configurationName);

  void setRobotTemplateConfigurationName(const std::string& robotName, const std::string& templateName);

  void setAutomaticallyConnect(const std::string &robotName, const bool doesConnectAutomatically);

  bool isAutomaticallyConnecting(const std::string &robotName);

  cedar::dev::RobotPtr getRobot(const std::string& robotName) const;

  const std::string& getRobotName(cedar::dev::ConstRobotPtr robot) const;

  std::string getNewRobotName() const;

  void addRobotTemplate(const std::string& name, const Template& robotTemplate);

  /*!@brief Get a list of all the robot templates known to the robot manager
   */
  std::vector<std::string> getRobotTemplateNames() const;

  /*!@brief Get a list of all the robots stored in the manager.
   */
  std::vector<std::string> getRobotNames() const;

  const Template& getTemplate(const std::string& name) const;

  void setRobotTemplateName(const std::string& robotName, const std::string& templateName);

  const std::string& getRobotTemplateName(const std::string& robotName) const;


  void writeRobotConfigurations(cedar::aux::ConfigurationNode& root) const;

  void readRobotConfigurations(cedar::aux::ConfigurationNode& robots);

  void connectRobotsAutomatically();


  inline boost::signals2::connection connectToRobotNameAddedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mRobotNameAddedSignal.connect(slot);
  }

  inline boost::signals2::connection connectToRobotConfigurationChangedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mRobotConfigurationLoadedSignal.connect(slot);
  }

  inline boost::signals2::connection connectToRobotRemovedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mRobotRemovedSignal.connect(slot);
  }

  /*!
   * @remarks Throws if no template has been loaded for the robot.
   */
  const std::string& getRobotTemplateConfigurationName(const std::string& robotName) const;

  /*!
   * @remarks Throws if no configuration is loaded for the robot.
   */
  const std::string& getRobotTemplateConfiguration(const std::string& robotName) const;

  /*!@brief  Finds the component slot specified by the given path.
   *         The slot must be given in the form "robot name.slot name".
   */
  cedar::dev::ComponentSlotPtr findComponentSlot(const std::string& componentPath) const;

  void emitConfigurationChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // signals
  //--------------------------------------------------------------------------------------------------------------------
  // signals a change in a robot's name; first parameter: old name, second: new name.
public:
  CEDAR_DECLARE_SIGNAL(RobotNameChanged, void (const std::string&, const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(ConfigurationChanged, void ());
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor. Private because this is a singleton object.
  RobotManager();

  RobotInfo& retrieveRobotInfo(const std::string& robotName)
  {
    auto iter = this->mRobotInfos.find(robotName);
    if (iter == this->mRobotInfos.end())
    {
      this->mRobotInfos[robotName] = RobotInfo();
      return this->mRobotInfos[robotName];
    }
    else
    {
      return iter->second;
    }
  }

  //! Saves the state of the robot manager to a file.
  void store() const;

  //! Restores the state of the robot manager.
  void restore();

  bool doesRobotExist(std::string robotName);

  void readKinematicChainConfigurations(std::string robotName,const cedar::aux::ConfigurationNode& robot);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::map<std::string, cedar::dev::RobotPtr> mRobotInstances;

  std::map<std::string, cedar::aux::Path> mRobotConfigurations;

  //! An association from robot names to the template that have been set for them.
  std::map<std::string, RobotInfo> mRobotInfos;

  boost::signals2::signal<void (const std::string&)> mRobotNameAddedSignal;

  boost::signals2::signal<void (const std::string&)> mRobotRemovedSignal;

  boost::signals2::signal<void (const std::string&)> mRobotConfigurationLoadedSignal;

  std::map<std::string, Template> mRobotTemplates;

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
    CEDAR_INSTANTIATE_DEV_TEMPLATE(cedar::aux::Singleton<cedar::dev::RobotManager>);
    //!@endcond
  }
}

#endif // CEDAR_DEV_ROBOT_MANAGER_H
