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

    File:        PluginDeclaration.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PLUGIN_DECLARATION_H
#define CEDAR_PROC_PLUGIN_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------
// macros
//--------------------------------------------------------------------------------------------------------------------

//!@todo Should CEDAR_BEGIN_PLUGIN_DECLARATION be in cedar/defines.h?
# ifdef __cplusplus
#   define CEDAR_BEGIN_PLUGIN_DECLARATION extern "C" {
#   define CEDAR_END_PLUGIN_DECLARATION }
# else
#   define CEDAR_BEGIN_PLUGIN_DECLARATION
#   define CEDAR_END_PLUGIN_DECLARATION
# endif

#ifdef _MSC_VER
#  define CEDAR_DECLARE_PROC_PLUGIN_FUNCTION(function_declaration) \
          CEDAR_BEGIN_PLUGIN_DECLARATION \
          __declspec(dllexport) function_declaration; \
          CEDAR_END_PLUGIN_DECLARATION
#else // _MSC_VER
#  define CEDAR_DECLARE_PROC_PLUGIN_FUNCTION(function_declaration) \
          CEDAR_BEGIN_PLUGIN_DECLARATION \
          function_declaration; \
          CEDAR_END_PLUGIN_DECLARATION
#endif // _MSC_VER

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
class cedar::proc::PluginDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of the element declaration list.
  typedef std::vector<cedar::proc::ElementDeclarationPtr> ElementDeclarations;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PluginDeclaration();

  //!@brief Destructor
  ~PluginDeclaration();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief add an ElementDeclaration to this plugin
  void add(cedar::proc::ElementDeclarationPtr declaration);

  //!@brief access all ElementDeclarations
  const ElementDeclarations& elementDeclarations() const;

  /*!@brief Reads parts of the plugin description from the given file.
   *
   * @remarks This function should only be called after all declarations have been added.
   */
  void readDescription(const std::string& filePath);

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
  void readElementDeclaration(const cedar::aux::ConfigurationNode& declarations);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief internal storage of all registered ElementDeclarations.
  ElementDeclarations mElementDeclarations;
}; // class cedar::proc::PluginDeclaration

#endif // CEDAR_PROC_PLUGIN_DECLARATION_H

