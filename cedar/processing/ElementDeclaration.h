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

    File:        ElementDeclaration.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_ELEMENT_DECLARATION_H
#define CEDAR_PROC_ELEMENT_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/processing/DeclarationBase.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/AbstractFactory.h"
#include "cedar/auxiliaries/AbstractFactoryDerived.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

/*!@brief A StepDeclaration contains the relation of a unique class id (as string) and the corresponding factory to
 * create a step of this id. It is a concretization of DeclarationBase.
 *
 */
class cedar::proc::ElementDeclaration : public cedar::proc::DeclarationBase
                                               <
                                                 cedar::proc::Element,
                                                 cedar::aux::AbstractFactory<cedar::proc::Element>
                                               >
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor.
  ElementDeclaration(
                      cedar::proc::ElementFactoryPtr classFactory,
                      const std::string& classId,
                      const std::string& category
                    )
  :
  DeclarationBase<cedar::proc::Element, cedar::aux::AbstractFactory<cedar::proc::Element> >(
                                                                                             classFactory,
                                                                                             classId,
                                                                                             category
                                                                                           )
  {
  }

  //!@brief Destructor
  ~ElementDeclaration()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief set path to icon included in the graphical representation of this element
  void setIconPath(const std::string& path)
  {
    this->mIconPath = path;
  }

  //!@brief get path to icon included in the graphical representation of this element
  const std::string& getIconPath() const
  {
    return this->mIconPath;
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief path to icon included in the graphical representation of this step
  std::string mIconPath;
};


/*!@brief This is a template class for comfortably generating element declarations.
 */
template <class DerivedClass>
class cedar::proc::ElementDeclarationTemplate : public ElementDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param category Category for the element.
   * @param classId  Identifier of the class. If this is left empty, the name will be determined automatically. For
   *                 example, a class test::namespaceName::ClassName will result in the name
   *                 test.namespaceName.ClassName.
   */

  ElementDeclarationTemplate(const std::string& category, const std::string& classId = "")
  :
  ElementDeclaration
  (
    cedar::proc::ElementFactoryPtr(new cedar::aux::AbstractFactoryDerived<cedar::proc::Element, DerivedClass>()),
    classId,
    category
  )
  {
    // if no class name is specified,
    if (classId.empty())
    {
      std::string class_name = cedar::aux::unmangleName(typeid(DerivedClass));
      class_name = cedar::aux::replace(class_name, "::", ".");
      this->setClassId(class_name);
    }
  }

  //!@brief Destructor
  ~ElementDeclarationTemplate()
  {
  }

  //!@brief checks if a generic Element is of a given child type (the template parameter of ElementDeclarationT)
  //!@param pointer step instance that is checked
  bool isObjectInstanceOf(cedar::proc::ElementPtr pointer)
  {
    return dynamic_cast<DerivedClass*>(pointer.get()) != NULL;
  }
}; // class cedar::proc::ElementDeclarationTemplate

#endif // CEDAR_PROC_ELEMENT_DECLARATION_H

