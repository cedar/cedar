/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

 File:        RecruitGroup.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 11 03

 Description: Source file for the class cedar::proc::scripts::RecruitGroup.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
// CLASS HEADER
#include "cedar/dynamics/scripts/ExecuteFunctionOnSteps.h"
// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/CppScriptDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

#include "cedar/dynamics/fields/Preshape.h"
#include "cedar/dynamics/steps/HebbianConnection.h"
// SYSTEM INCLUDES
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::CppScriptDeclarationPtr;
    using cedar::proc::CppScriptDeclarationTemplate;



    cedar::proc::CppScriptDeclarationPtr declaration
            (
                    new cedar::proc::CppScriptDeclarationTemplate<cedar::dyn::scripts::ExecuteFunctionOnSteps>("")
            );


    declaration->declare();

    return true;
  }

  bool declared = declare();
}



//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::dyn::scripts::ExecuteFunctionOnSteps::FunctionName> cedar::dyn::scripts::ExecuteFunctionOnSteps::FunctionName::mType(
        "FunctionName::"
                                                                                                                                                  );

cedar::dyn::scripts::ExecuteFunctionOnSteps::ExecuteFunctionOnSteps()
        :
        _mFunctionName(
                new cedar::aux::EnumParameter(this, "FunctionName",
                                              cedar::dyn::scripts::ExecuteFunctionOnSteps::FunctionName::typePtr(),
                                              FunctionName::ResetMemory
                                             ))
{
}

cedar::dyn::scripts::ExecuteFunctionOnSteps::~ExecuteFunctionOnSteps()
{
  this->requestStop();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::scripts::ExecuteFunctionOnSteps::run()
{

  switch (_mFunctionName->getValue())
  {
    case FunctionName::ResetMemory :
      this->preshapeResetMemory();
      break;
    case FunctionName::ResetHebbConWeights:
      this->hebbianConnectionResetWeights();
      break;
    default:
      std::cout << "Function was not specified!" << std::endl;
  }
}


void cedar::dyn::scripts::ExecuteFunctionOnSteps::hebbianConnectionResetWeights()
{
  std::set<cedar::dyn::steps::HebbianConnectionPtr> hebbianConnections = this->getGroup()->findAll<cedar::dyn::steps::HebbianConnection>(
          true);

  for (auto it = hebbianConnections.begin(); it != hebbianConnections.end(); it++)
  {
    auto hebbianConnection = *it;
    hebbianConnection->resetWeights();
  }
}

void cedar::dyn::scripts::ExecuteFunctionOnSteps::preshapeResetMemory()
{
  std::set<cedar::dyn::PreshapePtr> preShapes = this->getGroup()->findAll<cedar::dyn::Preshape>(
          true);

  for (auto it = preShapes.begin(); it != preShapes.end(); it++)
  {
    auto preshape = *it;
    preshape->resetMemory();
  }
}