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

    File:        TriggerDeclaration.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TRIGGER_DECLARATION_H
#define CEDAR_PROC_TRIGGER_DECLARATION_H

// LOCAL INCLUDES
#include "cedar/processing/Trigger.h"
#include "cedar/processing/namespace.h"
#include "cedar/processing/DeclarationBase.h"
#include "cedar/auxiliaries/Factory.h"
#include "cedar/auxiliaries/FactoryDerived.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief A TriggerDeclaration contains the relation of a unique class id (as string) and the corresponding factory to
 * create a trigger of this id. It is a concretization of DeclarationBase.
 *
 */
class cedar::proc::TriggerDeclaration : public cedar::proc::DeclarationBase
                                            <
                                              cedar::proc::Trigger,
                                              cedar::aux::Factory<cedar::proc::Trigger, cedar::proc::TriggerPtr>
                                            >
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  TriggerDeclaration(cedar::proc::TriggerFactoryPtr classFactory, const std::string& classId)
  :
  DeclarationBase<cedar::proc::Trigger, cedar::aux::Factory<cedar::proc::Trigger, cedar::proc::TriggerPtr> >
  (
    classFactory,
    classId,
    ""
  )
  {
  }

  virtual ~TriggerDeclaration()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief sets the path to icon file
  void setIconPath(const std::string& path)
  {
    this->mIconPath = path;
  }

  //!@brief returns the path to icon file
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
  //!@brief path to icon file
  std::string mIconPath;
};


/*!@brief Abstract description of the class with templates.
 *
 * More detailed description of the class with templates.
 */
template <class DerivedClass>
class cedar::proc::TriggerDeclarationT : public TriggerDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerDeclarationT(const std::string& classId)
  :
  TriggerDeclaration
  (
    cedar::proc::TriggerFactoryPtr(new cedar::aux::FactoryDerived<cedar::proc::Trigger, DerivedClass, cedar::proc::TriggerPtr>()),
    classId
  )
  {
  }

  //!@brief Destructor
  ~TriggerDeclarationT()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool isObjectInstanceOf(cedar::proc::TriggerPtr pointer)
  {
    return dynamic_cast<DerivedClass*>(pointer.get()) != NULL;
  }
}; // class cedar::proc::TriggerDeclarationT

#endif // CEDAR_PROC_TRIGGER_DECLARATION_H

