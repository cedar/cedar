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

    File:        EnumParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_PARAMETER_H
#define CEDAR_AUX_ENUM_PARAMETER_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/exceptions.h"
#include "auxiliaries/EnumBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <set>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::EnumParameter : public cedar::aux::ParameterBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  EnumParameter(const std::string& name,
                boost::shared_ptr<cedar::aux::EnumBase> enumBase);

  EnumParameter(const std::string& name,
                boost::shared_ptr<cedar::aux::EnumBase> enumBase,
                cedar::aux::EnumId defaultValue);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setTo(const cedar::aux::ConfigurationNode& root);

  void putTo(cedar::aux::ConfigurationNode& root);

  void makeDefault();

  cedar::aux::Enum get() const;

  void set(const std::string& enumId);

  const cedar::aux::EnumBase& getEnumDeclaration()
  {
    return *(this->mEnumDeclaration);
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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  cedar::aux::EnumId mValue;
  cedar::aux::EnumId mDefault;

  boost::shared_ptr<cedar::aux::EnumBase> mEnumDeclaration;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::VectorParameter

#endif // CEDAR_AUX_ENUM_PARAMETER_H

