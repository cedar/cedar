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
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <map>
#include <list>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/function.hpp>

/*!@brief An interface for classes that can store and load parameters from files.
 */
class cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Parameter;

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
  void registerParameter(cedar::aux::ParameterPtr parameter);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
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
}; // class cedar::aux::Configurable

#endif // CEDAR_AUX_CONFIGURABLE_H

