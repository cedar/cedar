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

    File:        main.cpp (fwCamInfo)

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 12 11

    Description: Short program to show the features of attached firewire cameras

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/LibDcBase.h"

// SYSTEM INCLUDES
#include <iostream>
#include <dc1394/dc1394.h>
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif


/*! A short program to display informations and capabilities of the properties of a given camera on the firewire bus
 */
int main(int argc, char *argv[])
{

  //default camera
  unsigned int camera_nr = 0;

  if (argc < 2)
  {
    std::cout << argv[0] << " shows all features of an attached firewire camera\n";
    std::cout << "\n\tUsage: " ;
    std::cout << "\t"<< argv[0] << " [camera_number]\n";
    std::cout << "\tIf no camera_number is present, the capabilities of\n"
              << "\tthe first camera on the bus will be displayed\n\n" << std::endl;
  }
  else
  {
    try
    {
      camera_nr = boost::lexical_cast<unsigned int>(argv[1]);
    }
    catch(boost::bad_lexical_cast &)
    {
      std::cout << "Please give a camera number to show the features of that camera\n";
      return -1;
    }
  }

  std::cout << "Informations on camera no. " << static_cast<int>(camera_nr) << "\n" <<  std::endl;

  //create firewire interface
  cedar::dev::sensors::camera::LibDcBase fw_interface;
  
  if (fw_interface.getNumCams() < 1)
  {
    std::cout << "No cameras found\n";
    fw_interface.cleanUp();
    return -1;
  }


  uint64_t guid;
  guid = fw_interface.getCamGuid(camera_nr);

  if (guid == 0)
  {
    std::cout << "Couldn't find camera with guid: " << guid << std::endl;
    return -1;
  }

  //--------------------------------------------------------------
  //open the cam
  fw_interface.openCamera(guid);
  
  //--------------------------------------------------------------
  //display Camera infos
  std::cout << "Camera:\n";

  //int cameraId = fw_camera->vendor_id;
  //std::cout << "Vendor_ID:\t" << cameraId << std::endl;
  std::cout << "GUID:\t" << int(guid)<<std::endl;
  std::cout << "GUID:\t0x" << std::hex << int(guid)<<std::endl;

  fw_interface.printAllFeatures();


  //--------------------------------------------------------------
  //print video modes
  std::cout << "\nSupported video modes:" << std::endl;

  dc1394video_modes_t cam_video_modes = fw_interface.getCamVideoModes();
  int num_modes = cam_video_modes.num;
  for (int i=0; i<num_modes; i++)
  {
    std::cout << " - " << cam_video_modes.modes[i]
                       <<" : "
                       << fw_interface.DC1394VideoModeToString(cam_video_modes.modes[i])
                       << std::endl;
  }

  //--------------------------------------------------------------
  //print framerates (depend on video mode)
  std::cout << "\n\nSupported framerates (depend on video mode):" << std::endl;
  for (int mode=0; mode<num_modes; mode++)
  {
    std::string video_mode_str = fw_interface.DC1394VideoModeToString(cam_video_modes.modes[mode]);
    std::cout << "\n - Video mode "
              << cam_video_modes.modes[mode]
              <<" : "
              << video_mode_str
              << std::endl;

    //no framerates on FORMAT7 modes
    if (cam_video_modes.modes[mode] >= DC1394_VIDEO_MODE_FORMAT7_0)
    {
      continue;
    }

    dc1394framerates_t mode_framerates = fw_interface.getCamFramerates(cam_video_modes.modes[mode]);
    int num_framerates = mode_framerates.num;
    for (int fr=0; fr<num_framerates; fr++)
    {
      std::cout << "\t - " << mode_framerates.framerates[fr]
                           << " : "
                           << fw_interface.DC1394FrameRateToString(mode_framerates.framerates[fr])
                           << std::endl;
    }
  }

  //cleanup
  fw_interface.closeCamera();

  return 0;
}
#endif //CEDAR_USE_LIB_DC1394
