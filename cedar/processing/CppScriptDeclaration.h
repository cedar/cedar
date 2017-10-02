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

    File:        CppScriptDeclaration.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 20

    Description: Header file for the class cedar::proc::CppScriptDeclaration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_CPP_SCRIPT_DECLARATION_H
#define CEDAR_PROC_CPP_SCRIPT_DECLARATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/processing/CppScript.fwd.h"
#include "cedar/processing/CppScriptDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief base class for scripts that can be executed in the gui
 */
class cedar::proc::CppScriptDeclaration : public cedar::aux::PluginDeclarationBaseTemplate<cedar::proc::CppScriptPtr>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CppScriptDeclaration(const std::string& category, const std::string& className = "");

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  virtual void declare() const = 0;

  std::string getPluginType() const;

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::CppScriptDeclaration

/*!@brief This is a template class for comfortably generating element declarations.
 *
 * @todo This class should get its own header file
 */
template <class DerivedClass>
class cedar::proc::CppScriptDeclarationTemplate
    :
    public cedar::aux::PluginDeclarationTemplate
    <
      cedar::proc::CppScriptPtr,
      boost::shared_ptr<DerivedClass>,
      cedar::proc::CppScriptDeclaration
    >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef
      cedar::aux::PluginDeclarationTemplate<cedar::proc::CppScriptPtr, boost::shared_ptr<DerivedClass>, cedar::proc::CppScriptDeclaration>
      DeclarationSuper;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param category Category for the script.
   * @param classId  Identifier of the class. If this is left empty, the name will be determined automatically. For
   *                 example, a class example::namespaceName::ClassName will result in the name
   *                 example.namespaceName.ClassName.
   */
  CppScriptDeclarationTemplate(const std::string& category, const std::string& classId = "")
  :
  DeclarationSuper
  (
    category,
    classId
  )
  {
  }

  void declare() const
  {
    DeclarationSuper::declare();
  }
}; // class cedar::proc::ElementDeclarationTemplate

#endif // CEDAR_PROC_CPP_SCRIPT_DECLARATION_H

