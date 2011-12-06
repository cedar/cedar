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

    File:        DeclarationBase.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DECLARATION_BASE_H
#define CEDAR_PROC_DECLARATION_BASE_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES


/*!@brief A base class for declarations of types that can be allocated dynamically.
 *
 *        This class can be used for dynamic allocation of objects of a certain type based on a string identifying the
 *        type.
 *
 * @todo Can this class be merged with ElementDeclaration?
 * @todo Should this be named cedar::proc::Declaration?
 */
template <class BaseClass, class FactoryType>
class cedar::proc::DeclarationBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Pointer type for the factory of BaseClass.
  typedef boost::shared_ptr<FactoryType> BaseFactoryPtr;

  //!@brief Pointer type for the BaseClass.
  typedef boost::shared_ptr<BaseClass> BasePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param classFactory  Pointer to the factory that creates objects declared in this definition.
   * @param classId       Unique name identifying the declared type.
   * @param category      Category of the type.
   */
  DeclarationBase
  (
    BaseFactoryPtr classFactory,
    const std::string& classId,
    const std::string& category = "misc."
  )
  :
  mpClassFactory(classFactory),
  mClassId(classId),
  mCategory (category)
  {
  }

  //!@brief The destructor.
  virtual ~DeclarationBase()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the object factory for this declaration.
   */
  BaseFactoryPtr getObjectFactory()
  {
    return this->mpClassFactory;
  }

  /*!@brief Returns the class id of the declaration.
   */
  const std::string& getClassId() const
  {
    return this->mClassId;
  }

  /*!@brief Returns the category of the declaration.
   */
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
  virtual bool isObjectInstanceOf(BasePtr) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Sets the class id of the declaration.
   */
  void setClassId(const std::string& classId)
  {
    this->mClassId = classId;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! The class factory pointer.
  BaseFactoryPtr mpClassFactory;

  //! The class id.
  std::string mClassId;

  //! The category.
  std::string mCategory;

}; // class cedar::proc::DeclarationBase

#endif // CEDAR_PROC_DECLARATION_BASE_H

