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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 22

    Description: Test convolution engine classes.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/testingUtilities/measurementFunctions.h"
#include "cedar/auxiliaries/convolution/OpenCV.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/configuration.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time.hpp>
#endif

struct TestSet
{
  TestSet(double sigma, double limit, unsigned int imsize, unsigned int reps, cedar::aux::conv::BorderType::Id borderType)
  :
  mSigma(sigma),
  mLimit(limit),
  mImsize(imsize),
  mReps(reps),
  mBorderType(borderType),
  mDuration(-1.0)
  {
  }

  std::string id() const
  {
    std::string case_id = "conv1d - sigma = " + cedar::aux::toString(this->mSigma)
                          + ", limit = " + cedar::aux::toString(this->mLimit)
                          + ", imsize = " + cedar::aux::toString(this->mImsize)
                          + ", reps = " + cedar::aux::toString(this->mReps)
                          + ", border = " + cedar::aux::conv::BorderType::type().get(this->mBorderType).name();
    return case_id;
  }

  double mSigma;
  double mLimit;
  unsigned int mImsize;
  unsigned int mReps;
  cedar::aux::conv::BorderType::Id mBorderType;
  double mDuration;
};

void test_convolution_time_1d(TestSet& test)
{
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;

  std::string case_id = test.id();

  cedar::aux::conv::ConvolutionPtr conv(new cedar::aux::conv::Convolution());
  conv->setBorderType(test.mBorderType);

  cedar::aux::kernel::GaussPtr gauss (new cedar::aux::kernel::Gauss(1, 1.0, test.mSigma, 0.0, test.mLimit));

  conv->getKernelList()->append(gauss);

  cv::Mat image = cv::Mat::ones(test.mImsize, 1, CV_32F);
  ptime start = microsec_clock::local_time();
  for (unsigned int i = 0; i < test.mReps; ++i)
  {
    // volatile so this doesn't get optimized away
    volatile cv::Mat test = conv->convolve(image);
  }
  ptime end = microsec_clock::local_time();

  test.mDuration = static_cast<double>((end - start).total_milliseconds()) / 1000.0;
  cedar::test::write_measurement(case_id, test.mDuration);
}


int main(int, char**)
{
  std::vector<TestSet> test;
  test.push_back(TestSet(10.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Zero));
  test.push_back(TestSet(100.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Zero));
  test.push_back(TestSet(1000.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Zero));
  test.push_back(TestSet(10000.0, 10.0, 100, 100, cedar::aux::conv::BorderType::Zero));
  test.push_back(TestSet(10.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Cyclic));
  test.push_back(TestSet(100.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Cyclic));
  test.push_back(TestSet(1000.0, 10.0, 100, 1000, cedar::aux::conv::BorderType::Cyclic));
  test.push_back(TestSet(10000.0, 10.0, 100, 100, cedar::aux::conv::BorderType::Cyclic));

  // measure
  for (size_t i = 0; i < test.size(); ++i)
  {
    test_convolution_time_1d(test[i]);
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
