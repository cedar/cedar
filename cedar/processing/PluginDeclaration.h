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

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------
// macros
//--------------------------------------------------------------------------------------------------------------------

//#ifdef GCC
# ifdef __cplusplus
#   define CEDAR_BEGIN_PLUGIN_DECLARATION extern "C" {
#   define CEDAR_END_PLUGIN_DECLARATION }
# else
#   define CEDAR_BEGIN_PLUGIN_DECLARATION
#   define CEDAR_END_PLUGIN_DECLARATION
# endif
/*#else // GCC
# define CEDAR_BEGIN_PLUGIN_DECLARATION
# define CEDAR_END_PLUGIN_DECLARATION
#endif*/

/*!@brief Collection of declarations from a plugin.
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
protected:
  // none yet
private:
  //!@brief internal storage of all registered ElementDeclarations.
  ElementDeclarations mElementDeclarations;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::PluginDeclaration

#endif // CEDAR_PROC_PLUGIN_DECLARATION_H

