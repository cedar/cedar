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

    File:        StepDeclarationT.h

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

#ifndef CEDAR_PROC_STEP_DECLARATION_H
#define CEDAR_PROC_STEP_DECLARATION_H

// LOCAL INCLUDES
#include "processing/DeclarationBase.h"
#include "processing/namespace.h"
#include "auxiliaries/AbstractFactory.h"
#include "auxiliaries/AbstractFactoryDerived.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::proc::StepDeclaration : public cedar::proc::DeclarationBase
                                            <
                                              cedar::proc::Step,
                                              cedar::aux::AbstractFactory<cedar::proc::Step>
                                            >
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  StepDeclaration(
                   cedar::proc::StepFactoryPtr classFactory,
                   const std::string& classId,
                   const std::string& category = "misc."
                 )
  :
  DeclarationBase<cedar::proc::Step, cedar::aux::AbstractFactory<cedar::proc::Step> >(classFactory, classId, category)
  {
  }

  ~StepDeclaration()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void setIconPath(const std::string& path)
  {
    this->mIconPath = path;
  }

  const std::string& getIconPath()
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
  std::string mIconPath;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet
};


/*!@brief Abstract description of the class with templates.
 *
 * More detailed description of the class with templates.
 */
template <class DerivedClass>
class cedar::proc::StepDeclarationT : public StepDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StepDeclarationT(
                    const std::string& classId,
                    const std::string& category = "misc."
                  )
  :
  StepDeclaration
  (
    cedar::proc::StepFactoryPtr(new cedar::aux::AbstractFactoryDerived<cedar::proc::Step, DerivedClass>()),
    classId,
    category
  )
  {
  }

  //!@brief Destructor
  ~StepDeclarationT()
  {
  }

  bool isObjectInstanceOf(cedar::proc::StepPtr pointer)
  {
    return dynamic_cast<DerivedClass*>(pointer.get()) != NULL;
  }
}; // class cedar::proc::StepDeclarationT

#endif // CEDAR_PROC_STEP_DECLARATION_H

