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

    File:        projection.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 06

    Description: Test projection performance.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/configuration.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"
#include "cedar/testingUtilities/measurementFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>

// helper method
void event_loop()
{
  int i = 0;

  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    QApplication::processEvents();
  }
}

void measure(unsigned int sourceDim, unsigned int targetDim, unsigned int repetitions)
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  using cedar::proc::steps::Projection;
  using cedar::proc::steps::ProjectionPtr;
  using cedar::proc::ProjectionMappingParameter;
  using cedar::proc::sources::GaussInput;
  using cedar::proc::sources::GaussInputPtr;

  GroupPtr network(new Group());
  network->readJson("base.json");

  auto projection = network->getElement<Projection>("projection");
  auto source = network->getElement<GaussInput>("source");

  if (sourceDim > 0) // dim 0 is a special case, the config file comes preconfigured with it
  {
    source->setDimensionality(sourceDim);
    for (unsigned int i = 0; i < sourceDim; ++i)
    {
      source->setSize(i, 90);
    }
  }

  projection->setOutputDimensionality(targetDim);

  auto mapping_parameter = projection->getParameter<ProjectionMappingParameter>("dimension mapping");

  for (unsigned int dim = 0; dim < targetDim; ++dim)
  {
    projection->setOutputDimensionSize(dim, 100);
  }

  // compression
  if (targetDim >= sourceDim)
  {
    for (unsigned int dim = 0; dim < sourceDim; ++dim)
    {
      mapping_parameter->changeMapping(dim, sourceDim - dim - 1);
    }
  }
  else if (targetDim < sourceDim)
  {
    for (unsigned int dim = 0; dim < targetDim; ++dim)
    {
      mapping_parameter->changeMapping(dim, targetDim - dim - 1);
    }
  }

  event_loop();

  std::string id;
  id += cedar::aux::toString(sourceDim);
  id += " -> ";
  id += cedar::aux::toString(targetDim);
  cedar::test::test_time
  (
    id,
    boost::bind(&cedar::proc::Step::onTrigger, projection, cedar::proc::ArgumentsPtr(), cedar::proc::TriggerPtr()),
    repetitions
  );

  if (projection->getState() == cedar::proc::Triggerable::STATE_EXCEPTION)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Configuration \"" + id + "\" resulted in an exception.",
      "void measure(unsigned int sourceDim, unsigned int targetDim)"
    );
  }
}

int main(int, char**)
{
  unsigned int max_dim = 3;
  for (unsigned int source = 0; source <= max_dim; ++source)
  {
    for (unsigned int target = 0; target <= max_dim; ++target)
    {
      unsigned int repetitions = 100;

      if (source > 2 || target > 2)
      {
        repetitions = 10;
      }

      measure(source, target, repetitions);
    }
  }
  return 0; // no errors -- this is a performance test.
}
