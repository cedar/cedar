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

    File:        Projection.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 12 05

    Description: Unit test for the cedar::proc::Projection class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/processing/StepTime.h"
#include "cedar/auxiliaries/logFilter/Type.h"
#include "cedar/auxiliaries/NullLogger.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES

/*!@brief Check whether the (only) projection of the network is in the given state.
 *
 * @param network the network our projection is a part of
 * @param state the state the projection should be in
 */
bool checkProjectionState(cedar::proc::GroupPtr& network, cedar::proc::Triggerable::State state)
{
  cedar::proc::steps::ProjectionPtr projection = network->getElement<cedar::proc::steps::Projection>("new Projection");
  if (projection->getState() == state)
  {
    return true;
  }

  return false;
}

/*!@brief Step the architecture of the given network.
 *
 * @param network the network that is to be stepped
 * @param numberOfErrors counter for the number of errors
 */
void stepArchitecture(cedar::proc::GroupPtr& network, unsigned int& numberOfErrors)
{
  try
  {
    cedar::proc::LoopedTriggerPtr trigger = network->getElement<cedar::proc::LoopedTrigger>("new LoopedTrigger");
    cedar::proc::ArgumentsPtr arguments
                              (
                                new cedar::proc::StepTime
                                    (
                                      cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second)
                                    )
                              );
    trigger->trigger(arguments);
  }
  catch (cedar::aux::ExceptionBase exception)
  {
    std::cout << "ERROR: An exception was thrown when stepping the loaded architecture.\n";
    std::cout << exception.getMessage() << "\n";
    ++numberOfErrors;
  }
}

/*!@brief Check the state of a valid projection.
 *
 * @param configurationFile name of the configuration file
 * @param numberOfErrors counter for the number of errors
 */
void checkValidProjection(const std::string& configurationFile, unsigned int& numberOfErrors)
{
  std::cout << "Checking file \"" << configurationFile << "\" (valid)" << std::endl;

  cedar::proc::GroupPtr network(new cedar::proc::Group());
  network->readJson("test://unit/processing/steps/Projection/" + configurationFile);

  // if the projection is in an invalid state, increase the error count
  if (checkProjectionState(network, cedar::proc::Triggerable::STATE_EXCEPTION) ||
      checkProjectionState(network, cedar::proc::Triggerable::STATE_UNKNOWN))
  {
    std::cout << "ERROR: Projection is not in a valid state.\n";
    ++numberOfErrors;
  }

  stepArchitecture(network, numberOfErrors);
}

/*!@brief Check the state of an invalid projection.
 *
 * @param configurationFile name of the configuration file
 * @param numberOfErrors counter for the number of errors
 */
void checkInvalidProjection(const std::string& configurationFile, unsigned int& numberOfErrors)
{
  std::cout << "Checking file \"" << configurationFile << "\" (invalid)" << std::endl;
  cedar::proc::GroupPtr network(new cedar::proc::Group());
  network->readJson("test://unit/processing/steps/Projection/" + configurationFile);

  if (!checkProjectionState(network, cedar::proc::Triggerable::STATE_EXCEPTION))
  {
    std::cout << "ERROR: Projection is not in an invalid state.\n";
    ++numberOfErrors;
  }

  stepArchitecture(network, numberOfErrors);
}

int main()
{
  // Filter out mem-debug messages so the output reamins readable
  cedar::aux::logFilter::TypePtr memdebug_filter(new cedar::aux::logFilter::Type(cedar::aux::LOG_LEVEL_MEM_DEBUG));
  cedar::aux::LogInterfacePtr memdebug_logger(new cedar::aux::NullLogger());
  cedar::aux::LogSingleton::getInstance()->addLogger(memdebug_logger, memdebug_filter);

  // count the number of errors
  unsigned int number_of_errors = 0;

  std::cout << "number of errors initialized to zero...\n";

  //===================================================================================================================
  // 1D to 0D
  //===================================================================================================================
  std::cout << "Checking 1D to 0D projections" << std::endl;
  checkValidProjection("configs/config_1Dto0D_average_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto0D_max_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto0D_min_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto0D_sum_valid.json", number_of_errors);

  //===================================================================================================================
  // 1D to 1D
  //===================================================================================================================
  std::cout << "Checking 1D to 1D projections" << std::endl;
  std::cout << "File: config_1Dto1D_0_valid.json" << std::endl;
  checkValidProjection("configs/config_1Dto1D_0_valid.json", number_of_errors);
  std::cout << "File: config_1Dto1D_drop_invalid.json" << std::endl;
  checkInvalidProjection("configs/config_1Dto1D_drop_invalid.json", number_of_errors);

  //===================================================================================================================
  // 1D to 2D
  //===================================================================================================================
  std::cout << "Checking 1D to 2D projections" << std::endl;
  checkValidProjection("configs/config_1Dto2D_0_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto2D_1_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_1Dto2D_drop_invalid.json", number_of_errors);

  //===================================================================================================================
  // 1D to 3D
  //===================================================================================================================
#ifdef CEDAR_USE_FFTW
  std::cout << "Checking 1D to 3D projections" << std::endl;
  checkValidProjection("configs/config_1Dto3D_0_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto3D_1_valid.json", number_of_errors);
  checkValidProjection("configs/config_1Dto3D_2_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_1Dto3D_drop_invalid.json", number_of_errors);
#endif // CEDAR_USE_FFTW

  //===================================================================================================================
  // 2D to 0D
  //===================================================================================================================
  std::cout << "Checking 2D to 0D projections" << std::endl;
  checkValidProjection("configs/config_2Dto0D_valid.json", number_of_errors);

  //===================================================================================================================
  // 2D to 1D
  //===================================================================================================================
  std::cout << "Checking 2D to 1D projections" << std::endl;
  checkInvalidProjection("configs/config_2Dto1D_0_0_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto1D_0_drop_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto1D_drop_0_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto1D_drop_drop_invalid.json", number_of_errors);

  //===================================================================================================================
  // 2D to 2D
  //===================================================================================================================
  std::cout << "Checking 2D to 2D projections" << std::endl;
  checkInvalidProjection("configs/config_2Dto2D_0_0_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto2D_0_1_average_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto2D_0_1_maximum_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto2D_0_1_minimum_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto2D_0_1_sum_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_0_drop_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto2D_1_0_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_1_1_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_1_drop_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_drop_0_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_drop_1_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto2D_drop_drop_invalid.json", number_of_errors);

#ifdef CEDAR_USE_FFTW

  //===================================================================================================================
  // 2D to 3D
  //===================================================================================================================
  std::cout << "Checking 2D to 3D projections" << std::endl;
  checkInvalidProjection("configs/config_2Dto3D_0_0_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_0_1_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_0_2_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_0_drop_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_1_0_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_1_1_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_1_2_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_1_drop_invalid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_2_0_valid.json", number_of_errors);
  checkValidProjection("configs/config_2Dto3D_2_1_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_2_2_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_2_drop_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_drop_0_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_drop_1_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_drop_2_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_2Dto3D_drop_drop_invalid.json", number_of_errors);

  //===================================================================================================================
  // 3D to 0D
  //===================================================================================================================
  std::cout << "Checking 3D to 0D projections" << std::endl;
  checkValidProjection("configs/config_3Dto0D_valid.json", number_of_errors);

  //===================================================================================================================
  // 3D to 1D
  //===================================================================================================================
  std::cout << "Checking 3D to 1D projections" << std::endl;
  checkInvalidProjection("configs/config_3Dto1D_0_0_0_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_3Dto1D_0_0_drop_invalid.json", number_of_errors);
  checkInvalidProjection("configs/config_3Dto1D_0_drop_0_invalid.json", number_of_errors);
  checkValidProjection("configs/config_3Dto1D_0_drop_drop_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_3Dto1D_drop_0_0_invalid.json", number_of_errors);
  checkValidProjection("configs/config_3Dto1D_drop_0_drop_valid.json", number_of_errors);
  checkValidProjection("configs/config_3Dto1D_drop_drop_0_valid.json", number_of_errors);
  checkInvalidProjection("configs/config_3Dto1D_drop_drop_drop_invalid.json", number_of_errors);

  //===================================================================================================================
  // 3D to 2D
  //===================================================================================================================
  std::cout << "Checking 3D to 2D projections" << std::endl;
  checkValidProjection("configs/config_3Dto2D_0_1_drop_valid.json", number_of_errors);
  checkValidProjection("configs/config_3Dto2D_drop_1_0_valid.json", number_of_errors);

  //===================================================================================================================
  // 3D to 3D
  //===================================================================================================================
  std::cout << "Checking 3D to 3D projections" << std::endl;
  checkValidProjection("configs/config_3Dto3D_0_1_2_valid.json", number_of_errors);
  checkValidProjection("configs/config_3Dto3D_1_2_0_valid.json", number_of_errors);
#endif // CEDAR_USE_FFTW

  // check strange bugs!
  cedar::proc::GroupPtr group(new cedar::proc::Group());
  group->create("cedar.processing.sources.GaussInput", "gauss");
  group->create("cedar.processing.Projection", "projection");
  group->connectSlots("gauss.Gauss input","projection.input");
  auto projection = group->getElement<cedar::proc::steps::Projection>("projection");
  auto map = projection->getParameter<cedar::proc::ProjectionMappingParameter>("dimension mapping");
  map->getValue()->changeMapping(0, 0);
  projection->setOutputDimensionality(0);

  std::cout << "Done. There were " << number_of_errors << " errors.\n";

  return number_of_errors;
}
