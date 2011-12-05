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
#include "cedar/processing/Trigger.h"
#include "cedar/processing/MultiTrigger.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Network.h"
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <QReadWriteLock>

class ComputableTest : public cedar::proc::Step
{
  public:
    ComputableTest
    (
      unsigned int sequenceId,
      std::vector<unsigned int>& sequenceBuffer,
      QReadWriteLock& lock,
      bool runInThread
    )
    :
    cedar::proc::Step(runInThread),
    mSequenceId (sequenceId),
    meSequenceBuffer (sequenceBuffer),
    meLock (lock)
    {
    }

    void compute(const cedar::proc::Arguments& /* arguments */)
    {
      //! @todo log these times and calculate the overall test time to check performance.
      cedar::aux::usleep(rand() % 50000);
      meLock.lockForWrite();
      meSequenceBuffer.push_back(mSequenceId);
      meLock.unlock();
    }

    unsigned int mSequenceId;
    std::vector<unsigned int>& meSequenceBuffer;
    QReadWriteLock& meLock;
};

typedef boost::shared_ptr<ComputableTest> ComputableTestPtr;

class EndStopTest : public cedar::proc::Step
{
  public:
    EndStopTest()
    :
    cedar::proc::Step(false),
    mFinished (false)
    {
    }

    void compute(const cedar::proc::Arguments& /* arguments */)
    {
      mFinished = true;
    }

    bool mFinished;
};

typedef boost::shared_ptr<EndStopTest> EndStopTestPtr;

// =====================================================================================================================

cedar::aux::LogFile log_file ("SequenceTest.log");

/*
 * Tests the correct generation of a sequence.
 *
 * The sequence is specified as a sequence string: for example, in the string
 *    111232311
 * each number specifies the number of steps to execute in parallel before going to the next stage (i.e., the next
 * number).
 */
void testSequence(const std::string& sequenceString, unsigned int& errors, bool runInThread)
{
  using cedar::proc::Trigger;
  using cedar::proc::TriggerPtr;
  using cedar::proc::MultiTrigger;
  using cedar::proc::MultiTriggerPtr;

  QReadWriteLock lock;
  std::vector<unsigned int> sequence_buffer;

  log_file << "Creating computation chain for sequence string \"" << sequenceString << "\"." << std::endl;


  log_file << "This test is ";

  if (!runInThread)
  {
    log_file << "not ";
  }
  log_file << "threaded." << std::endl;

  TriggerPtr sequence_trigger(new Trigger("sequence_trigger"));

  TriggerPtr prev_trigger = sequence_trigger;
  std::string desired_sequence = "";

  log_file << "Creating network ... " << std::endl;
  cedar::proc::NetworkPtr network (new cedar::proc::Network());
  /*
   * Generate the sequence in the processing framework
   */
  unsigned int num_steps = 0;
  for (size_t i = 0; i < sequenceString.size(); ++i)
  {
    MultiTriggerPtr multi_trigger(new MultiTrigger());

    std::string chr = "";
    chr += sequenceString.at(i);
    int no_steps = atoi(chr.c_str());
    if (no_steps <= 0)
    {
      log_file << "Malformed sequence string \"" << sequenceString << "\" at position " << i <<
          ": Cannot convert to number, or number is less than 1." << std::endl;
      return;
    }
    std::vector<ComputableTestPtr> steps;

    for (int no = 0; no < no_steps; ++no)
    {
      ComputableTestPtr step (new ComputableTest(i, sequence_buffer, lock, runInThread));
      steps.push_back(step);
      network->connectTrigger(prev_trigger, step);
      network->connectTrigger(step->getFinishedTrigger(), multi_trigger);

      std::stringstream strstr;
      strstr << i;
      desired_sequence += strstr.str();
      ++num_steps;
    }
    prev_trigger = multi_trigger;
  }

  EndStopTestPtr stop_test(new EndStopTest());
  network->add(stop_test);
  network->connectTrigger(prev_trigger, stop_test);

  log_file << "Triggering sequence " << sequenceString << "." << std::endl;
  sequence_trigger->trigger();

  unsigned int wait_time_in_sec = 3;
  unsigned int wait_iterations = 0;
  unsigned int usec_wait_per_iteration = 1000;
  while (!stop_test->mFinished && wait_iterations * usec_wait_per_iteration < 1000000 * wait_time_in_sec)
  {
    cedar::aux::usleep(usec_wait_per_iteration);
    ++wait_iterations;
  }

  if (!stop_test->mFinished)
  {
    log_file << "The test timed out." << std::endl;
    ++errors;
    return;
  }


  /*
   * Test the sequence for correctness
   */
  log_file << "Checking sequence " << sequenceString << "." << std::endl;
  unsigned int prev_stage_no = sequence_buffer.at(0);
  std::string sequence = "";
  for (size_t i = 0; i < sequence_buffer.size(); ++i)
  {
    unsigned int stage_no = sequence_buffer.at(i);
    // an error occurs if the stage number is more than one bigger than the last one
    if (stage_no != prev_stage_no + 1 && stage_no != prev_stage_no)
    {
      log_file << "Wrong order detected in sequence " << sequenceString << "." << std::endl;
      ++errors;
    }
    prev_stage_no = stage_no;

    std::stringstream strstr;
    strstr << stage_no;
    sequence += strstr.str();
  }

  if (desired_sequence != sequence)
  {
    log_file << "Wrong sequence generated for " << sequenceString << "." << std::endl;
    log_file << "Generated sequence is: " << sequence << "." << std::endl;
    log_file << "Desired sequence is: " << desired_sequence << "." << std::endl;
    ++errors;
  }
  else
  {
    log_file << "Generated sequence is ok! *thumbs up*" << std::endl;
  }
}


int main(int /* argc */, char** /* argv */)
{
  using cedar::aux::LogFile;
  using cedar::proc::Trigger;
  using cedar::proc::TriggerPtr;

  unsigned int errors = 0;

  log_file.addTimeStamp();
  log_file << std::endl;

  testSequence("112319", errors, false);

  testSequence("112319", errors, true);

  log_file << "Done. There were " << errors << " errors." << std::endl;

  return errors;
}
