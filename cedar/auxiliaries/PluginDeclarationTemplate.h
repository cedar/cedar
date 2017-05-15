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

    File:        PluginDeclarationTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PLUGIN_DECLARATION_TEMPLATE_H
#define CEDAR_AUX_PLUGIN_DECLARATION_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/DeclarationManagerTemplate.h"
#include "cedar/auxiliaries/boostConstPointerHelper.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/PluginDeclarationTemplate.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief A template for plugin declarations.
 *
 * @tparam BaseClass The direct base class for this class. This class must either be or inherit
 *         cedar::aux::PluginDeclarationBaseTemplate<BaseClassPtr>.
 */
template <class BaseClassPtr, class PluginClassPtr, class BaseClass = cedar::aux::PluginDeclarationBaseTemplate<BaseClassPtr> >
class cedar::aux::PluginDeclarationTemplate
:
public BaseClass,
public boost::enable_shared_from_this<cedar::aux::PluginDeclarationTemplate<BaseClassPtr, PluginClassPtr, BaseClass> >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef typename cedar::aux::Singleton<cedar::aux::FactoryManager<BaseClassPtr> > PluginFactoryManager;
  typedef typename cedar::aux::Singleton<cedar::aux::DeclarationManagerTemplate<BaseClassPtr> > DeclarationManager;
  typedef typename PluginClassPtr::element_type PluginClass;
  typedef typename cedar::aux::ConstPtrProvider<BaseClassPtr>::ConstBaseTypePtr ConstBaseTypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param category  Category of the declaration.
   * @param className Class name to use. A name will automatically be generated when this is left empty.
   *
   * @remarks The className parameter is essentially a legacy parameter to provide downward-compatibility; it should
   *          usually be left empty.
   */
  PluginDeclarationTemplate(const std::string& category, const std::string& className = std::string())
  :
  BaseClass(category, className)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Declares this plugin at the appropriate factory.
   */
  void declare() const
  {
    DeclarationManager::getInstance()->addDeclaration(this->shared_from_this());

    for (size_t i = 0; i < this->deprecatedNames().size(); ++i)
    {
      PluginFactoryManager::getInstance()->addDeprecatedName(this->getClassName(), this->deprecatedNames().at(i));
    }

    this->onDeclare();
  }

  /*! Returns name of the class stored in this declaration.
   */
  std::string getClassName() const
  {
    if (!this->mClassName.empty())
    {
      return this->mClassName;
    }
    else
    {
      return PluginFactoryManager::getInstance()->template generateTypeName<PluginClassPtr>();
    }
  }

  /*!@brief Checks if an Element is of the plugin class's type.
   *
   * @param pointer Instance that is checked
   */
  bool isObjectInstanceOf(ConstBaseTypePtr pointer) const
  {
    if (pointer)
    {
      return typeid(PluginClass) == typeid(*pointer);
    }
    return false;
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
  virtual void onDeclare() const
  {
    PluginFactoryManager::getInstance()->template registerType<PluginClassPtr>(this->mClassName);

    if (this->isDeprecated())
    {
      PluginFactoryManager::getInstance()->template deprecate<PluginClassPtr>();
    }
  }

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

}; // class cedar::aux::PluginDeclarationTemplate

#endif // CEDAR_AUX_PLUGIN_DECLARATION_TEMPLATE_H

