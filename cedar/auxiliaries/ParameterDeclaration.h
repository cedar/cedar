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

    File:        Parameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PARAMETER_DECLARATION_H
#define CEDAR_AUX_PARAMETER_DECLARATION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/PluginDeclarationTemplate.h"
#include "cedar/auxiliaries/FactoryManager.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ParameterDeclaration.fwd.h"

// SYSTEM INCLUDES


// declarations of managers
namespace cedar
{
  namespace aux
  {
    //! Factory manager used for parameters.
    typedef
        cedar::aux::FactoryManager<cedar::aux::ParameterPtr>
        ParameterFactoryManager;

    //! Declaration manager for parameters.
    typedef
        cedar::aux::DeclarationManagerTemplate<cedar::aux::ParameterPtr>
        ParameterDeclarationManager;
  }
}

CEDAR_AUX_SINGLETON(ParameterFactoryManager);
CEDAR_AUX_SINGLETON(ParameterDeclarationManager);

/*! Declaration used for exposing parameters to cedar's plugin system.
 */
template <typename ParameterTypePtr>
class cedar::aux::ParameterDeclaration
:
public cedar::aux::PluginDeclarationTemplate<cedar::aux::ParameterPtr, ParameterTypePtr>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::PluginDeclarationTemplate<cedar::aux::ParameterPtr, ParameterTypePtr> Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ParameterDeclaration(const std::string& category = std::string(), const std::string& classId = std::string())
  :
  Super(category, classId)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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
};


#endif // CEDAR_AUX_PARAMETER_DECLARATION_H
