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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      
 ----- Email:       
 ----- Date:        2010 12 04

 ----- Description: Implements all unit tests for the @em cedar::aux::net

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP


// LOCAL INCLUDES
#include "cedar/auxiliaries/net/BlockingReader.h"
#include "cedar/auxiliaries/net/Reader.h"
#include "cedar/auxiliaries/net/Writer.h"
#include "cedar/auxiliaries/net/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <string>
#include <sstream>

using namespace cedar::aux::net;
using namespace std;

int main()
{
  // the number of errors encountered in this test
  signed char errors = 0;
  signed char mat_errors = 0;
  
  //// Test for simple variable passing ////
  try
  {
    float fl;

#define MYSIMPLEPORT "CEDAR-UNIT-TEST-SIMPLE"
    Writer<float> myFloatWriter(MYSIMPLEPORT);
    BlockingReader<float> myFloatReader(MYSIMPLEPORT);

    fl= 42.42;
    myFloatWriter.write(fl);

    if (myFloatReader.read() != fl)
    {
      mat_errors++;
    }
  }
  catch (cedar::aux::ExceptionBase &E)
  {
    std::cout << E.exceptionInfo() << std::endl;
    return 1;
  }

  if (mat_errors)
  {
    errors++;
    std::cout << "ERROR with float" << std::endl;
    mat_errors= 0;
  }
  else
  {
    std::cout << "OK (float)" << std::endl;
  }

  //// cv::Mat Test ////
  typedef double TestType;
#define SIZE 5000
#define MYPORT "CEDAR-UNIT-TEST"

#if 1
  // preparing ...
  cv::Mat mat = cv::Mat::eye(SIZE, 2, CV_64F);
  cv::Mat mat2 = cv::Mat::zeros(SIZE, 2, CV_64F);
  cv::Mat mat3 = cv::Mat::zeros(16, 2, CV_64F);
  cv::Mat mat6 = cv::Mat::zeros(SIZE, 2, CV_64F);
  cv::Mat mat7 = cv::Mat::zeros(SIZE, 2, CV_64F);
  cv::Mat mat8 = cv::Mat::zeros(20, 2, CV_32FC3); // multichannel
  cv::Mat mat9 = cv::Mat::zeros(20, 2, CV_32FC3); // multichannel

  for (int i=0; i < SIZE; i++)
  {
    mat.at<TestType>(i,0) = 300 + i;
    mat.at<TestType>(i,1) = SIZE + 500 + i;

    mat6.at<TestType>(i,0) = i;
    mat6.at<TestType>(i,1) = 100 + i;

    mat2.at<TestType>(i,0) = -1;
    mat2.at<TestType>(i,1) = -1;
  }
  // mat8 is a bit smaller, but has multi channels
  for (int i = 0; i < 20; ++i)
  {
    cv::Vec3f rgb;
    rgb[0] = 1.0;
    rgb[1] = 2.0;
    rgb[2] = static_cast<float>(i);
    mat8.at<cv::Vec3f>(i,0) = rgb;
    mat8.at<cv::Vec3f>(i,1) = rgb;
  }

  try
  {
    // writing ...
    Writer<cv::Mat> myMatWriter(MYPORT);
    Writer<cv::Mat> myMatWriter8(MYPORT"8");

    // ... reading
    BlockingReader<cv::Mat> myMatReader2(MYPORT);
    BlockingReader<cv::Mat> myMatReader3(MYPORT);
    BlockingReader<cv::Mat> myMatReader8(MYPORT"8");

    myMatWriter.write(mat);
    mat2 = myMatReader2.read();
    mat3 = myMatReader3.read();

    // writing over same channel (mat6 is same size as mat)
    myMatWriter.write(mat6);
    mat7 = myMatReader2.read();

    // test it
    for (int i=0; i < SIZE; i++)
    {
      if (mat2.at<TestType>(i,0) != 300 + i
          || mat2.at<TestType>(i,1) != SIZE + 500 +i
          // auch mat3 testen ...
          || mat3.at<TestType>(i,0) != 300 + i
          || mat3.at<TestType>(i,1) != SIZE + 500 +i
          // auch mat7 testen
          || mat7.at<TestType>(i,0) != i
          || mat7.at<TestType>(i,1) != 100 +i
        )
      {
        // FAIL
        mat_errors++;
      }
    }

    myMatWriter8.write(mat8);
    mat9 = myMatReader8.read();

    for (int i= 0; i < 20; i++)
    {
      cv::Vec3f rgb;

      rgb = mat9.at<cv::Vec3f>(i,0);

      if (rgb[2] != i)
      {
        mat_errors++;
      }
    }

  } // end try
  catch (cedar::aux::ExceptionBase &E)
  {
    std::cout << E.exceptionInfo() << std::endl;
    return 1;
  }

  if (mat_errors)
  {
    std::cout << "ERROR with cv::Mat simple write() / read()" << std::endl;
    errors++;
    mat_errors = 0;
  }
  else
  {
    std::cout << "OK (cv::Mat)" << std::endl;
  }

#endif

#if 1
  //// cv::Mat_ (note the underscore!) Test ////
  try
  {
    typedef cv::Mat_<float> floatMat_;
    floatMat_ mat4, mat5;
  
    mat_errors = 0;
    Writer< cv::Mat_<float> > myMatWriter2_1(MYPORT"_");
    BlockingReader< cv::Mat_<float> > myMatReader2_1(MYPORT"_");

    mat4 = floatMat_(2,2);


    for (int i = 0; i < mat4.rows; i++)
    {
      for (int j = 0; j < mat4.cols; j++)
      {
        mat4(i,j)= 79;
      }
   }

    myMatWriter2_1.write(mat4);
    mat5 = myMatReader2_1.read();


    if (mat5(1,1) != mat4(1,1))
    {
      mat_errors++;
      std::cout << "ERROR with cv::Mat_ test" << std::endl;
    }
  }
  catch (cedar::aux::net::NetWaitingForWriterException &e)
  {
    std::cout << "ERROR writer not initialized yet, should wait" << std::endl;
    return 1;
  }
  catch (cedar::aux::net::NetMissingRessourceException &e)
  {
    std::cout << "ERROR YARP NAME SERVER NOT RUNNING" << std::endl;
    return 1;
  }
  catch (cedar::aux::net::NetUnexpectedDataException &e)
  {
    std::cout << "ERROR YARP unexpected data" << std::endl;
    return 1;
  }
#endif
 
  if (mat_errors)
  {
    std::cout << "ERROR with cv::Mat_ simple" << std::endl;
    errors++;
    mat_errors = 0;
  }
  else
  {
    std::cout << "OK (cv::Mat_)" << std::endl;
  }

#if 1
  // std::string test
  try
  {
    std::string s = "hello world";

#define MYSTRINGPORT "CEDAR-UNIT-TEST-STRING"
    Writer<std::string> myStringWriter(MYSTRINGPORT);
    BlockingReader<std::string> myStringReader(MYSTRINGPORT);

    myStringWriter.write(s);

    std::string t;
    t = myStringReader.read();
    if (t != s)
    {
      mat_errors++;
      std::cout << "ERROR with std::string (1)" << std::endl;
    }

    // test sending another string (with different size)
    std::string s2 = "lalala";
    myStringWriter.write(s2);
    std::string t2;
    t2 = myStringReader.read();

    if (t2 != s2)
    {
      mat_errors++;
      std::cout << "ERROR with std::string (2)" << std::endl;
    }

  }
  catch (cedar::aux::ExceptionBase &E)
  {
    std::cout << E.exceptionInfo() << std::endl;
    return 1;
  }
#endif
  std::cout << "done, encountered " << static_cast<unsigned int>(errors) << " errors" << std::endl;

  return errors;
}

#endif // CEDAR_USE_YARP

