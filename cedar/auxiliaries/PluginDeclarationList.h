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

// FORWARD DECLARATION
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
#include "cedar/auxiliaries/PluginDeclarationList.fwd.h"
#include "cedar/auxiliaries/PluginProxy.fwd.h"

// SYSTEM INCLUDES
#include <vector>

// definitions used for loading a list from a plugin.
# ifdef __cplusplus
#   define CEDAR_BEGIN_PLUGIN_DECLARATION extern "C" {
#   define CEDAR_END_PLUGIN_DECLARATION }
# else
#   define CEDAR_BEGIN_PLUGIN_DECLARATION
#   define CEDAR_END_PLUGIN_DECLARATION
# endif

// Macro for declaring the plugin function
#ifdef CEDAR_OS_WINDOWS
#  define CEDAR_DECLARE_PROC_PLUGIN_FUNCTION(function_declaration) \
          CEDAR_BEGIN_PLUGIN_DECLARATION \
          __declspec(dllexport) function_declaration; \
          CEDAR_END_PLUGIN_DECLARATION
#else // CEDAR_OS_WINDOWS
#  define CEDAR_DECLARE_PROC_PLUGIN_FUNCTION(function_declaration) \
          CEDAR_BEGIN_PLUGIN_DECLARATION \
          function_declaration; \
          CEDAR_END_PLUGIN_DECLARATION
#endif // CEDAR_OS_WINDOWS

/*!@brief Collection of declarations from a plugin.
 *
 * Some information about the declaration can also be stored in a file accompanying the plugin binary. The file should
 * be located in the same folder as the binary, or one level above and must have the same name as the plugin.
 * Example:
 * if your plugin binary is located in /home/you/src/MyPluginLib/build/libMyPlugin.so, then the plugin description file can
 * be either /home/you/src/MyPluginLib/build/MyPlugin.xml or /home/you/src/MyPluginLib/MyPlugin.xml.
 *
 * The contents of the file should look like this:
 * @code
<?xml version="1.0" encoding="UTF-8" ?>

<plugin>
  <declarations>

    <!-- the following declares properties of the class yourNamespace::YourClassName that you should
         have added to the plugin declaration in your plugin.cpp -->
    <element class="yourNamespace.YourClassName">
      <icon>:/icons/the_icon.svg</icon>
      <description>Description of the step.</description>
    </element>

  </declarations>
</plugin>
 * @endcode
 * Note, that you should always use Qt's resource system to store icons and other resources in order to avoid confusion
 * about filepaths.
 */
class cedar::aux::PluginDeclarationList
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::PluginProxy;

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

  //! Returns the number of declarations in this list.
  size_t size() const
  {
    return this->mDeclarations.size();
  }

  //! Returns the declaration at the given index.
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

  //! Sets the source of the plugin list. Only set after the list has been populated!
  void setSource(const std::string& source);

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

