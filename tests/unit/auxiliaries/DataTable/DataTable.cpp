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

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/DataTable.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/stringFunctions.h"

#define CEDAR_TEST_CONDITION(expr) if (!(expr)) { std::cout << "ERROR: test condition " << #expr << " in line " << __LINE__ << " failed." << std::endl; ++errors;} \
    else {std::cout << "passed test condition " << #expr << std::endl;}

// SYSTEM INCLUDES

void writeCollection(cedar::aux::DataTable::ConstRowCollectionPtr collection)
{
  for (const auto& row : *collection)
  {
    bool first = true;
    for (const auto& id : collection->getTable()->listIds())
    {
      if (first)
      {
        first = false;
      }
      else
      {
        std::cout << ", ";
      }
      std::cout << id << ": " << row->getId(id) << std::endl;
    }
  }
}

int basic_test()
{
  int errors = 0;

  cedar::aux::DataTablePtr table(new cedar::aux::DataTable());
  table->createCounter("trial");
  table->createCounter("instance");
  table->createId("instance");
  table->createDataColumn("mat data");

  unsigned int ntrials = 3;
  unsigned int ninstances = 3;

  for (unsigned int trial = 0; trial < ntrials; ++trial)
  {
    for (unsigned int instance = 0; instance < ninstances; ++instance)
    {
      auto row = table->getTable()->appendNewRow();
      row->setCounter("trial", trial);
      row->setCounter("instance", instance);
      row->setId("instance", "instance" + cedar::aux::toString(instance));

      cedar::aux::MatDataPtr mat_data(new cedar::aux::MatData(cv::Mat::zeros(ninstances, 1, CV_32F)));
      mat_data->getData().at<float>(instance, 0) = 1.0;
      row->setData("mat data", mat_data);

      if (instance % 2 == 0)
      {
        // add a tag
        row->addTag("test1");
      }
      else
      {
        // add a different tag
        row->addTag("test2");
      }
    }
  }

  // -- Test different selection methods
  
  // select by tag
  auto test1_tagged = table->getTable()->selectByTag("test1");
  CEDAR_TEST_CONDITION(test1_tagged->size() == (ntrials * (1 + ninstances / 2)));

  // select by counter
  {
    auto instance_0_entries = table->getTable()->selectByCounter("instance", 0);
    CEDAR_TEST_CONDITION(instance_0_entries->size() == ntrials);
  }

  // select by id
  {
    auto instance_0_entries = table->getTable()->selectById("instance", "instance0");
    CEDAR_TEST_CONDITION(instance_0_entries->size() == ntrials);
  }

  // -- Test summing over subsets --

  // sum over all instance numbers
  {
    auto collection = table->getTable()->selectByCounter("instance", 2);
    double instances_sum = collection->sum
    (
      [](cedar::aux::DataTable::ConstRowPtr row)
      {
        return static_cast<double>(row->getCounterValue("instance"));
      }
    );
    CEDAR_TEST_CONDITION(instances_sum == static_cast<double>(ntrials * 2));
  }

  // -- Test averaging over subsets --
  {
    auto collection = table->getTable()->selectByCounter("instance", 2);
    double instances_avg = collection->average
      (
      [](cedar::aux::DataTable::ConstRowPtr row)
    {
      return static_cast<double>(row->getCounterValue("instance"));
    }
    );
    CEDAR_TEST_CONDITION(instances_avg == 2.0);
  }

  //!@todo -- Test histogram computation --
  // (not yet implemented; should be able to generate a histogram of values returned by a lambda function. Example: rank distribution in object recognition, i.e., for each recognition event, calculate the rank of the correct recognition, then compute the histogram of that to see the distribution

  //! -- Test usage of (mat) data --
  // average over location of maximum of instance 1 samples (avg. should be 1, all indices are placed to correspond to the instance number)
  {
    auto collection = table->getTable()->selectByCounter("instance", 1);
    double instances_avg = collection->average
      (
      [](cedar::aux::DataTable::ConstRowPtr row)
    {
      auto data = row->getData("mat data");
      if (!data)
      {
        return -1.0;
      }
      auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
      if (!mat_data)
      {
        return -1.0;
      }

      cv::Point max_loc;
      cv::minMaxLoc(mat_data->getData(), nullptr, nullptr, nullptr, &max_loc);
      return static_cast<double>(max_loc.y);
    }
    );
    CEDAR_TEST_CONDITION(instances_avg == 1.0);
  }

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += basic_test();

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
