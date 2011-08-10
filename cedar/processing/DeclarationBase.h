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

// LOCAL INCLUDES
#include "processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <class BaseClass, class FactoryType>
class cedar::proc::DeclarationBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef boost::shared_ptr<FactoryType> BaseFactoryPtr;
  typedef boost::shared_ptr<BaseClass> BasePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
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

  //!@brief Destructor
  virtual ~DeclarationBase()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  BaseFactoryPtr getObjectFactory()
  {
    return this->mpClassFactory;
  }

  const std::string& getClassId() const
  {
    return this->mClassId;
  }

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  BaseFactoryPtr mpClassFactory;
  std::string mClassId;
  std::string mCategory;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::DeclarationBase

#endif // CEDAR_PROC_DECLARATION_BASE_H

