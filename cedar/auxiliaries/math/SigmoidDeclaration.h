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

    File:        SigmoidDeclarationT.h

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

#ifndef CEDAR_AUX_MATH_SIGMOID_DECLARATION_H
#define CEDAR_AUX_MATH_SIGMOID_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/Factory.h"
#include "cedar/auxiliaries/FactoryDerived.h"

// SYSTEM INCLUDES


/*!@brief A SigmoidDeclaration contains the relation of a unique class id (as string) and the corresponding factory to
 *        create a sigmoid of this id. It is a concretization of DeclarationBase.
 */
class cedar::aux::math::SigmoidDeclaration
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief constructor
  SigmoidDeclaration(
                      cedar::aux::math::SigmoidFactoryPtr classFactory,
                      const std::string& classId,
                      const std::string& category = "misc."
                    )
  :
  mpClassFactory(classFactory),
  mClassId(classId),
  mCategory (category)
  {
  }

  //!@brief destructor
  virtual ~SigmoidDeclaration()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get a factory for this declared sigmoid type
  cedar::aux::math::SigmoidFactoryPtr getObjectFactory()
  {
    return this->mpClassFactory;
  }

  //!@brief get the id of the declared sigmoid class
  const std::string& getClassId() const
  {
    return this->mClassId;
  }

  //!@brief get the category defined in this declaration
  const std::string& getCategory() const
  {
    return this->mCategory;
  }

  /*!
   * @brief Returns the class name without the preceding namespace.
   */
  std::string getClassName() const
  {
    std::size_t index = this->getClassId().rfind('.');
    if (index != std::string::npos)
    {
      return this->getClassId().substr(index + 1);
    }
    else
    {
      return this->getClassId();
    }
  }

  /*!
   * @brief Returns the namespace name without the class name.
   */
  std::string getNamespaceName() const
  {
    std::size_t index = this->getClassId().rfind('.');
    if (index != std::string::npos)
    {
      return this->getClassId().substr(0, index);
    }
    else
    {
      return this->getClassId();
    }
  }

  /*!
   * @returns True, if the object passed as argument is an instance of the class represented by this declaration, false
   *          otherwise.
   */
  virtual bool isObjectInstanceOf(cedar::aux::math::SigmoidPtr) = 0;

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
  //!@brief pointer to a factory, which can allocate instances of ths declared class
  cedar::aux::math::SigmoidFactoryPtr mpClassFactory;
  //!@brief a unique class id
  std::string mClassId;
  //!@brief a category
  std::string mCategory;
private:
  // none yet
};

/*!@brief Template for convenient declaration of sigmoid classes.
 */
template <class DerivedClass>
class cedar::aux::math::SigmoidDeclarationT : public cedar::aux::math::SigmoidDeclaration
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SigmoidDeclarationT(
                    const std::string& classId,
                    const std::string& category = "misc."
                  )
  :
  SigmoidDeclaration
  (
    cedar::aux::math::SigmoidFactoryPtr(new cedar::aux::FactoryDerived<cedar::aux::math::SigmoidPtr, DerivedClass>()),
    classId,
    category
  )
  {
  }

  //!@brief Destructor
  ~SigmoidDeclarationT()
  {
  }

  //!@brief check if a given ptr to a sigmoid is of this declaration's type
  bool isObjectInstanceOf(cedar::aux::math::SigmoidPtr pointer)
  {
    return dynamic_cast<DerivedClass*>(pointer.get()) != NULL;
  }
}; // class cedar::aux::math::SigmoidDeclarationT

#endif // CEDAR_AUX_MATH_SIGMOID_DECLARATION_H
