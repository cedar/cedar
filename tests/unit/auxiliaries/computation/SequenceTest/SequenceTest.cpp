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
#include "auxiliaries/computation/MultiTrigger.h"
#include "auxiliaries/computation/ProcessingStep.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <QReadWriteLock>

class ComputableTest : public cedar::aux::comp::ProcessingStep
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
void testSequence(const std::string& sequenceString, unsigned int& errors)
{
  using cedar::aux::comp::Trigger;
  using cedar::aux::comp::TriggerPtr;
  using cedar::aux::comp::MultiTrigger;
  using cedar::aux::comp::MultiTriggerPtr;

  QReadWriteLock lock;
  std::vector<unsigned int> sequence_buffer;

  log_file << "Creating computation chain for sequence string \"" << sequenceString << "\"." << std::endl;

  TriggerPtr sequence_trigger(new Trigger());

  TriggerPtr prev_trigger = sequence_trigger;
  std::string desired_sequence = "";

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
      ComputableTestPtr step (new ComputableTest(i, sequence_buffer, lock));
      steps.push_back(step);
      prev_trigger->addListener(step);

      step->getFinishedTrigger()->addTrigger(multi_trigger);

      std::stringstream strstr;
      strstr << i;
      desired_sequence += strstr.str();
      ++num_steps;
    }
    prev_trigger = multi_trigger;
  }

  log_file << "Triggering sequence " << sequenceString << "." << std::endl;
  sequence_trigger->trigger();

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
}


int main(int argc, char** argv)
{
  using cedar::aux::LogFile;
  using cedar::aux::comp::Trigger;
  using cedar::aux::comp::TriggerPtr;

  unsigned int errors = 0;

  log_file.addTimeStamp();
  log_file << std::endl;

  testSequence("112311", errors);

  log_file << "Done. There were " << errors << " errors." << std::endl;

  return errors;
}
