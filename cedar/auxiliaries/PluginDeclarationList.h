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

    File:        PluginDeclarationList.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PLUGIN_DECLARATION_LIST_H
#define CEDAR_AUX_PLUGIN_DECLARATION_LIST_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::aux::PluginDeclarationList
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef std::vector<cedar::aux::PluginDeclarationPtr> DeclarationList;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PluginDeclarationList();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Add a PluginDeclaration to this list.
  void add(cedar::aux::PluginDeclarationPtr declaration);

  /*!@brief   Reads parts of the plugin description from the given xml file.
   *
   * @remarks This function should only be called after all declarations have been added.
   */
  void readDescription(const std::string& filePath);

  /*!@brief Returns the PluginDeclaration corresponding to the given class name.
   */
  cedar::aux::PluginDeclarationPtr findPluginDeclaration(const std::string& className) const;

  //! Declares all plugins in this list.
  void declareAll() const;

  size_t size() const
  {
    return this->mDeclarations.size();
  }

  cedar::aux::PluginDeclarationPtr at(size_t i) const
  {
    return this->mDeclarations.at(i);
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
  void readDeclarations(const cedar::aux::ConfigurationNode& declarations);
  void readDeclaration(const cedar::aux::ConfigurationNode& declarations);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! List of the element declarations stored in this plugin declaration.
  DeclarationList mDeclarations;

}; // class cedar::aux::PluginDeclarationList

#endif // CEDAR_AUX_PLUGIN_DECLARATION_LIST_H

