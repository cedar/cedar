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

    File:        main.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Unit test for the behavior of the GrabberInterface (stereo-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/devices/sensors/visual/grabbertests/TestGrabber.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!
 *  \file This file implements a test case for the GrabberInterface class for a stereo grabber
 */

int main(int , char **)
{
  
  const std::string CHANNEL_0_NAME = "CHANNEL_0_NAME";
  const std::string CHANNEL_1_NAME = "CHANNEL_1_NAME";
  const std::string GRABBER_NAME = "TestGrabber";
  const std::string CONFIG_FILE_NAME = "grabber.config";
  const std::string LOGFILE = "UnitTestStereoGrabber.log";


  //create logfile
  cedar::aux::LogFile log_file(LOGFILE);
  log_file.addTimeStamp();
  log_file << std::endl;

  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;


  //------------------------------------------------------------------------------------------------------------------
  //create a stereo-test-grabber (only a dummy grabber for testing)
  cedar::dev::sensors::visual::TestGrabber *grabber_1 = new cedar::dev::sensors::visual::TestGrabber
                                                            (
                                                              CONFIG_FILE_NAME,
                                                              CHANNEL_0_NAME,
                                                              CHANNEL_1_NAME
                                                            );

  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": setName() and getName()" << std::endl;
  try
  {
    std::string name = "NewTestGrabber";
    grabber_1->setName(name);
    if (grabber_1->getName() != name ) {throw (-1);}
  }
  catch(...)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": setFps() and getFps()" << std::endl;

  try
  {
    grabber_1->setFps(40);
    if (grabber_1->getFps() != 40)
    {
      throw ( -1 );
    }
  }
  catch (...)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": getFpsMeasured()" << std::endl;
  try
  {
    grabber_1->getFpsMeasured();
  }
  catch (...)
  {
    log_file << "error" <<std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //test default parameters
  log_file << "test no " << test_number++ <<": default-parameter" << std::endl;
  if (grabber_1->getTestParam() != 123)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //stereo-grabber specific

  //-----------------------------------------------------------
  //test number of channels
  log_file << "test no " << test_number++ <<": getNumCams" << std::endl;

  if (grabber_1->getNumCams() != 2 )
  {
    log_file << "error" << std::endl;
    errors++;
  }


  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": setRecordName() and getRecordName()" << std::endl;

  //the default extension used in cedar. please check this.
  try
  {
    std::string name,result,expected;
    std::string ext = cedar::dev::sensors::visual::GrabberInterface::mGrabberDefaultRecordExtension;

    //set one w/o ext
    name="RecordNameNewName(0)";
    grabber_1->setRecordName(0,name);
    result = grabber_1->getRecordName(0);
    expected = name + ext;
    if ( result != expected ) {throw (-1);}

    name="RecordNameNewName(1)";
    grabber_1->setRecordName(1,name);
    result = grabber_1->getRecordName(1);
    expected = name + ext;
    if ( result != expected ) {throw (-1);}

    //set one w/ ext
    name="RecordNameNewName(0).avi";
    grabber_1->setRecordName(0,name);

    result = grabber_1->getRecordName(0);
    expected = name;
    if (result != expected) {throw (-1);}

    name="RecordNameNewName(1).avi";
    grabber_1->setRecordName(1,name);
    result = grabber_1->getRecordName(1);
    expected = name;
    if (result != expected) {throw (-1);}

    //set all w/o ext
    name="RecordNameNewName_all";
    grabber_1->setRecordName(name);
    result = grabber_1->getRecordName(0);
    expected = name+grabber_1->getChannelSaveFilenameAddition(0)+ext;
    if (result != expected) {throw (-1);}

    grabber_1->setRecordName(name);
    result = grabber_1->getRecordName(1);
    expected = name+grabber_1->getChannelSaveFilenameAddition(1)+ext;
    if (result != expected) {throw (-1);}



    //set all w/ ext
    name="RecordNameNewName_all";
    grabber_1->setRecordName(name+ext);
    result = grabber_1->getRecordName(0);
    expected = name+grabber_1->getChannelSaveFilenameAddition(0)+ext;
    if (result != expected) {throw (-1);}

    result = grabber_1->getRecordName(1);
    expected = name+grabber_1->getChannelSaveFilenameAddition(1)+ext;
    if (result != expected) {throw (-1);}
  }
  catch (...)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": isRecording()" << std::endl;
  try
  {
    grabber_1->isRecording();
  }
  catch (...)
  {
    log_file << "error" <<std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": setSnapshotName() and getSnapshotName()" << std::endl;
  try
  {
    std::string name,result,expected;
    const std::string ext = cedar::dev::sensors::visual::GrabberInterface::mGrabberDefaultSnapshotExtension ;

    //set one w/o ext
    name="SnapshotNameNewName(0)";
    grabber_1->setSnapshotName(0,name);
    if (grabber_1->getSnapshotName(0) != name+ext )
      {throw (-1);}

    name="SnapshotNameNewName(1)";
    grabber_1->setSnapshotName(1,name);
    if (grabber_1->getSnapshotName(1) != name+ext )
      {throw (-1);}

    //set one w/ ext
    name="SnapshotNameNewName(0).avi";
    grabber_1->setSnapshotName(0,name);
    if (grabber_1->getSnapshotName(0) != name) {throw (-1);}

    name="SnapshotNameNewName(1).avi";
    grabber_1->setSnapshotName(1,name);
    if (grabber_1->getSnapshotName(1) != name) {throw (-1);}

    //set all w/o ext
    name="SnapshotNameNewName_all";
    grabber_1->setSnapshotName(name);

    result = grabber_1->getSnapshotName(0);
    expected = name+grabber_1->getChannelSaveFilenameAddition(0)+ext;
    if (result != expected) {throw (-1);}

    result = grabber_1->getSnapshotName(1);
    expected = name+grabber_1->getChannelSaveFilenameAddition(1)+ext;
    if (result != expected) {throw (-1);}


    //set all w/ ext
    name="SnapshotNameNewName_all";
    grabber_1->setSnapshotName(name+ext);

    result = grabber_1->getSnapshotName(0);
    expected = name+grabber_1->getChannelSaveFilenameAddition(0)+ext;
    if (result != expected) {throw (-1);}

    result = grabber_1->getSnapshotName(1);
    expected = name+grabber_1->getChannelSaveFilenameAddition(1)+ext;
    if (result != expected) {throw (-1);}

  }
  catch (...)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //readWriteLockPointer
  log_file << "test no " << test_number++ <<": getReadWriteLockPointer()" << std::endl;
  try
  {
    QReadWriteLock *result = grabber_1->getReadWriteLockPointer();
    if (result == NULL) { throw ( -1 ); }
  }
  catch (...)
  {
    log_file << "error" << std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //test the rest of the get-methods
  log_file << "test no " << test_number++ <<": getSize() and getSourceInfo()" << std::endl;
  try
  {

    grabber_1->getSize();
    grabber_1->getSize(0);
    grabber_1->getSize(1);

    std::string result;

    result = grabber_1->getSourceInfo();
    if (result.empty()) { throw (-1); }


    result = grabber_1->getSourceInfo(0);
    if (result.empty()) { throw (-1); }

    result = grabber_1->getSourceInfo(1);
    if (result.empty()) { throw (-1); }
  }
  catch (...)
  {
    log_file << "error" << std::endl;
    errors++;
  }


  //-----------------------------------------------------------
  log_file << "test no " << test_number++ <<": getReadWriteLockPointer()" << std::endl;
  try
  {
    grabber_1->getReadWriteLockPointer();
  }
  catch (...)
  {
    log_file << "error" <<std::endl;
    errors++;
  }

  //-----------------------------------------------------------
  //check errors
  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }

  return errors;

}

