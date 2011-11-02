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

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <list>

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
  typedef std::list<cedar::aux::ParameterPtr> ParameterList;
  typedef std::map<std::string, ParameterList::iterator> ParameterMap;
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
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);
  void readJson(const std::string& filename);
  virtual void writeConfiguration(cedar::aux::ConfigurationNode& root);
  void writeJson(const std::string& filename);
  const Children& configurableChildren() const;

  const ParameterList& getParameters() const;
  ParameterList& getParameters();

  void addConfigurableChild(const std::string& name, cedar::aux::ConfigurablePtr child);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  virtual void configurationLoaded();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void registerParameter(cedar::aux::ParameterPtr parameter);

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
protected:
  // none yet

private:
  ParameterList mParameterOrder;
  ParameterMap mParameterAssociations;
  Children mChildren;
  // none yet

}; // class cedar::aux::Configurable

#endif // CEDAR_AUX_CONFIGURABLE_H

