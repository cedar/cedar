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

    File:        SequenceTest.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/computation/Trigger.h"
#include "auxiliaries/computation/Computable.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <iostream>
#include <sstream>
#include <vector>
#include <QReadWriteLock>

class ComputableTest : public cedar::aux::comp::Computable
{
  public:
    ComputableTest(unsigned int sequenceId, std::vector<unsigned int>& sequenceBuffer, QReadWriteLock& lock)
    :
    mSequenceId (sequenceId),
    meSequenceBuffer (sequenceBuffer),
    meLock (lock)
    {
    }

    void compute(const cedar::aux::comp::Arguments& arguments)
    {
      meLock.lockForWrite();
      meSequenceBuffer.push_back(mSequenceId);
      meLock.unlock();
    }

    unsigned int mSequenceId;
    std::vector<unsigned int>& meSequenceBuffer;
    QReadWriteLock& meLock;
};

typedef boost::shared_ptr<ComputableTest> ComputableTestPtr;



int main(int argc, char** argv)
{
  using cedar::aux::LogFile;
  using cedar::aux::comp::Trigger;
  using cedar::aux::comp::TriggerPtr;

  unsigned int errors = 0;

  LogFile log_file("SequenceTest.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  QReadWriteLock lock;
  std::vector<unsigned int> sequence_buffer;

  log_file << "Creating computation chain." << std::endl;
  std::vector<ComputableTestPtr> steps;
  for (size_t i = 0; i < 4; ++i)
  {
    steps.push_back(ComputableTestPtr(new ComputableTest(i, sequence_buffer, lock)));
  }

  TriggerPtr start_trigger(new Trigger());

  start_trigger->addListener(steps[0]);

  steps[0]->getFinishedTrigger().addListener(steps[1]);
  steps[0]->getFinishedTrigger().addListener(steps[2]);

  log_file << "Executing computation chain." << std::endl;

  start_trigger->trigger();

  log_file << "Computation chain finished." << std::endl;
  std::string sequence = "";

  for (size_t i = 0; i < sequence_buffer.size(); ++i)
  {
    std::stringstream strstr;
    strstr << sequence_buffer.at(i);
    sequence += strstr.str();
  }
  log_file << "Generated sequence:" << sequence << std::endl;

  log_file << "Done. There were " << errors << " errors." << std::endl;

  return errors;
}
