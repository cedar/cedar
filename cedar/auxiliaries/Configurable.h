/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Configurable.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONFIGURABLE_H
#define CEDAR_AUX_CONFIGURABLE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES
#include <map>
#include <list>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

/*!@brief An interface for classes that can store and load parameters from files.
 */
class cedar::aux::Configurable : public boost::noncopyable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Parameter;
  friend class cedar::aux::ConfigurationInterface;

  //--------------------------------------------------------------------------------------------------------------------
  // macros and types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief list of cedar::aux::ParameterPtr
  typedef std::list<cedar::aux::ParameterPtr> ParameterList;
  //!@brief map of parameter names to its place in ParameterList, expressed as iterator
  typedef std::map<std::string, ParameterList::iterator> ParameterMap;
  //!@brief map of children of a single cedar::aux::Configurable, pointing from their name to their pointers
  typedef std::map<std::string, cedar::aux::ConfigurablePtr> Children;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Configurable();

  //!@brief Destructor
  virtual ~Configurable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  /*!@brief create a tree of type cedar::aux::ConfigurationNode from a json file and try to read the configuration
   * afterward
   */
  void readJson(const std::string& filename);

  /*!@brief   Reads the configuration from an INI file.
   *
   * @remarks This method is used for providing downward compatibility to the old config interface. Please don't use it
   *          for anything else!
   */
  void readOldConfig(const std::string& filename);

  /*!@brief   Writes the configuration to an INI file.
   *
   * @remarks This method is used for providing downward compatibility to the old config interface. Please don't use it
   *          for anything else!
   */
  void writeOldConfig(const std::string& filename);

  //!@brief write a configuration for all registered parameters to a root node of type cedar::aux::ConfigurationNode
  virtual void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //!@brief write a configuration to a cedar::aux::ConfigurationNode tree and store this tree in a json file
  void writeJson(const std::string& filename) const;

  //!@brief get a map of all children of the current Configurable
  const Children& configurableChildren() const;

  //!@brief get a const list of all parameters registered at this Configurable
  const ParameterList& getParameters() const;

  //!@brief get a list of all parameters registered at this Configurable
  ParameterList& getParameters();

  /*!@brief add a Configurable as a child to this instance of Configurable - if name is duplicate, an exception is
   * thrown
   */
  void addConfigurableChild(const std::string& name, cedar::aux::ConfigurablePtr child);

  //!@brief removes a child configuration from the configuration tree - if name is not found, an exception is thrown
  void removeConfigurableChild(const std::string& name);

  /*!@brief register a function pointer with this function to react to any changes in the tree structure (for example,
   * removing a child node)
   */
  boost::signals2::connection connectToTreeChangedSignal(boost::function<void ()> slot);

  /*!@brief Resets the changed flag of all parameters associated with this Configurable.
   */
  void resetChangedStates(bool newChangedFlagValue) const;

  //!@brief copy a configuration from another instance of the same class (type check included)
  void copyFrom(ConstConfigurablePtr src);

  //!@brief copy a configuration to another instance of the same class (type check included)
  void copyTo(ConfigurablePtr target) const;

  /*!@brief Returns the parameter associated with the path.
   * @todo This should be const, but that conflicts with the intrusive pointers
   */
  cedar::aux::ParameterPtr getParameter(const std::string& path);

  /*!@brief Returns the configurable child associated with the path.
   * @todo This should be const, but that conflicts with the intrusive pointers in getParameter
   */
  cedar::aux::ConfigurablePtr getConfigurableChild(const std::string& path);

  /*!@brief Locks all parameters of the configurable.
   */
  void lockParameters(cedar::aux::LOCK_TYPE lockType);

  /*!@brief Unlocks all parameters of the configurable.
   */
  void unlockParameters();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief emits a "changed" signal for every parameter that was loaded
  virtual void configurationLoaded();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief register a new parameter at this Configurable - throws an exception if parameter is already part of this
   * Configurable
   */
  void registerParameter(cedar::aux::Parameter* parameter);

  //!@brief Transforms the old config format to one readable in the new interface.
  void oldFormatToNew(cedar::aux::ConfigurationNode& node);

  //!@brief Transforms the old config format to one readable in the new interface.
  void newFormatToOld(cedar::aux::ConfigurationNode& node);

  //!@brief Sets all parameters to their default values.
  void defaultAll();

  //!@brief Updates the lock set of the configurable.
  void updateLockSet();

  //!@brief Appends the locks of this configurable and its children to the set.
  void appendLocks(std::set<QReadWriteLock*>& locks);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief a boost signal that is emitted if a change in the tree structure appears
  boost::signals2::signal<void ()> mTreeChanged;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief list of parameters registered at this Configurable instance
  ParameterList mParameterList;

  //!@brief map of registered parameters, using the parameter name as key
  ParameterMap mParameterAssociations;

  //!@brief map of children of this Configurable instance
  Children mChildren;

  /*!@brief   The lockable used for locking this configurable and all its parameters.
   *
   * @remarks In order to avoid multiple inheritance down the line, configurable has, rather than is, a lockable.
   */
  std::set<QReadWriteLock*> mParameterLocks;
}; // class cedar::aux::Configurable

#endif // CEDAR_AUX_CONFIGURABLE_H
