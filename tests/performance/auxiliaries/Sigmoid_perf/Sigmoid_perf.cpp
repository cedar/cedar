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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 01 13

    Description: Test convolution engine classes.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/testingUtilities/measurementFunctions.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/configuration.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time.hpp>
#endif

struct TestSet
{
  TestSet(unsigned dim, unsigned int imsize, unsigned int reps)
  :
  mDimensionality(dim),
  mMatrixSize(imsize),
  mReps(reps),
  mDuration(-1.0)
  {
  }

  std::string id() const
  {
    std::string case_id = "transfer - dimensionality = " + cedar::aux::toString(this->mDimensionality)
                          + ", matrix size = " + cedar::aux::toString(this->mMatrixSize)
                          + ", reps = " + cedar::aux::toString(this->mReps);
    return case_id;
  }

  unsigned int mDimensionality;
  unsigned int mMatrixSize;
  unsigned int mReps;
  double mDuration;
};

void test_sigmoid(TestSet& test)
{
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;

  std::string case_id = test.id();

  cedar::aux::math::TransferFunctionPtr sigmoid(new cedar::aux::math::AbsSigmoid());

  std::vector<int> sizes;
  for (unsigned int dim = 0; dim < test.mDimensionality; ++dim)
  {
    sizes.push_back(static_cast<int>(test.mMatrixSize));
  }
  cv::Mat matrix(static_cast<int>(test.mDimensionality), &(sizes.front()), CV_32F);
  matrix = 1.0;

  ptime start = microsec_clock::local_time();
  for (unsigned int i = 0; i < test.mReps; ++i)
  {
    // volatile so this doesn't get optimized away
    volatile cv::Mat test = sigmoid->compute<float>(matrix);
  }
  ptime end = microsec_clock::local_time();
  test.mDuration = static_cast<double>((end - start).total_milliseconds()) / 1000.0;
  cedar::test::write_measurement(case_id, test.mDuration);
}

int main(int, char**)
{
  std::vector<TestSet> test;
//  test.push_back(TestSet(1, 100, 100));
  test.push_back(TestSet(2, 20, 100));
  test.push_back(TestSet(2, 100, 10));
  test.push_back(TestSet(3, 20, 100));
  test.push_back(TestSet(3, 100, 10));
  // measure
  for (size_t i = 0; i < test.size(); ++i)
  {
    test_sigmoid(test[i]);
  }

  // summarize results
  for (size_t i = 0; i < test.size(); ++i)
  {
    std::cout << test[i].id()
              << " \t|\t" << test[i].mDuration << " s"
              << std::endl;
  }
  return 0;
}
