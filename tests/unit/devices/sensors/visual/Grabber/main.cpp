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

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Unit test for the behavior of the Grabber (mono case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/testingUtilities/devices/TestGrabber.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif

/*!
 *  \file This file implements a test case for the Grabber class
 */

//--------------------------------------------------------------------------------------------------------------------
//main program
//--------------------------------------------------------------------------------------------------------------------
int main(int , char **)
{

  const std::string CHANNEL_0_NAME = "CHANNEL_0_NAME";
  const std::string GRABBER_NAME = "NewTestGrabber";
  const std::string CONFIG_FILE_NAME = "grabber.config";
  const std::string GRABBER_PARAMETER_1 = "file1.png";

  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;


  //------------------------------------------------------------------------------------------------------------------
  //create a test-grabber (only a dummy grabber for testing)
  cedar::test::dev::TestGrabber* pGrabber = new cedar::test::dev::TestGrabber(GRABBER_PARAMETER_1);


  //-----------------------------------------------------------
  // create configfile
  std::cout << "test no " << test_number++ <<": writeJson()" << std::endl;
  try
  {
    pGrabber->writeJson(CONFIG_FILE_NAME);
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  // read from configfile
  std::cout << "test no " << test_number++ <<": readJson()"  << std::endl;
  try
  {
    pGrabber->readJson(CONFIG_FILE_NAME);
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": applyParameter()" << std::endl;
  try
  {
    pGrabber->applyParameter();
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }


  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": setName() and getName()" << std::endl;
  try
  {
    pGrabber->setName(GRABBER_NAME);
    if (pGrabber->getName() != GRABBER_NAME ) {throw (-1);}
  }
  catch(...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": setFps() and getFps()" << std::endl;
  try
  {
    pGrabber->setFramerate(40);
    if (pGrabber->getFramerate() != 40)
    {
      throw ( -1 );
    }
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": getFpsMeasured()" << std::endl;
  try
  {
    pGrabber->getMeasuredFramerate();
  }
  catch (...)
  {
    std::cout << "error" <<std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //test default parameters
  std::cout << "test no " << test_number++ <<": default-parameter" << std::endl;
  pGrabber->setTestParam(123);
  if (pGrabber->getTestParam() != 123)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //test number of channels
  std::cout << "test no " << test_number++ <<": getNumCams" << std::endl;

  if (pGrabber->getNumChannels() != 1 )
  {
    std::cout << "error" << std::endl;
    errors++;
  }


  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": setRecordName() and getRecordName()" << std::endl;
  try
  {
    std::string name,result;
    const std::string ext = cedar::dev::sensors::visual::Grabber::mGrabberDefaultRecordExtension;

    //set one w/o ext
    name="RecordNameNewName(0)";
    pGrabber->setRecordName(0,name);
    result = pGrabber->getRecordName();
    if ( result != name+ext ) {throw (-1);}
    result =  pGrabber->getRecordName(0);
    if ( result != name+ext ) {throw (-1);}


    //set one w/ ext
    name="RecordNameNewName(0).avi";
    pGrabber->setRecordName(0,name);
    result = pGrabber->getRecordName();
    if (result != name) {throw (-1);}

    //set all w/o ext
    name="RecordNameNewName_all";
    pGrabber->setRecordName(name);
    result = pGrabber->getRecordName();
    if (result != name+ext) {throw (-1);}


    //set all w/ ext
    name="RecordNameNewName_all.avi";
    pGrabber->setRecordName(name);
    if (pGrabber->getRecordName()!=name) {throw (-1);}
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": isRecording()" << std::endl;
  try
  {
    pGrabber->isRecording();
  }
  catch (...)
  {
    std::cout << "error" <<std::endl;
    errors++;
  }


  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": setSnapshotName() and getSnapshotName()" << std::endl;
  try
  {
    std::string name, result, expected;
    const std::string ext = cedar::dev::sensors::visual::Grabber::mGrabberDefaultSnapshotExtension;

    //set one w/o ext
    name="SnapshotNameNewName(0)";
    pGrabber->setSnapshotName(0,name);
    result = pGrabber->getSnapshotName();
    expected = name+ext;
    if (result != expected ) {throw (-1);}

    result = pGrabber->getSnapshotName(0);
    expected = name+ext;
    if (result != expected ) {throw (-1);}

    //set one w/ ext
    name="SnapshotNameNewName(0).jpg";
    pGrabber->setSnapshotName(0,name);
    result = pGrabber->getSnapshotName();
    expected = name;
    if (result != expected ) {throw (-1);}

    //set all w/o ext
    name="SnapshotNameNewName_all";
    pGrabber->setSnapshotName(name);
    if (pGrabber->getSnapshotName()!=name+ext) {throw (-1);}

    //set all w/ ext
    name="SnapshotNameNewName_all.jpg";
    pGrabber->setSnapshotName(name);
    if (pGrabber->getSnapshotName()!=name) {throw (-1);}

  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //readWriteLockPointer
  std::cout << "test no " << test_number++ <<": getReadWriteLockPointer()" << std::endl;
  try
  {
    QReadWriteLock *result = pGrabber->getReadWriteLockPointer();
    if (result == NULL) { throw ( -1 ); }
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": getSize()" << std::endl;
  try
  {
    pGrabber->getSize();
    pGrabber->getSize(0);
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": getSourceInfo()" << std::endl;
  try
  {
    std::string result;

    result = pGrabber->getSourceInfo();
    if (result.empty()) { throw (-1); }


    result = pGrabber->getSourceInfo(0);
    if (result.empty()) { throw (-1); }
  }
  catch (...)
  {
    std::cout << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  std::cout << "test no " << test_number++ <<": getReadWriteLockPointer()" << std::endl;
  try
  {
    pGrabber->getReadWriteLockPointer();
  }
  catch (...)
  {
    std::cout << "error" <<std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //check errors
  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }

  //delete configfile
  boost::filesystem::remove(CONFIG_FILE_NAME);

  return errors;
}

