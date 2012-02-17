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

    File:        saveFwCamFeatures

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 12 11

    Description: Short program to save the properties of all attached firewire cameras to a configuration file

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/grabbertools/FirewireInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <dc1394/dc1394.h>
#include <boost/lexical_cast.hpp>
#include <fstream>

// ------------------------------------------------------------------------------------------------------------------
// The filename where the capabilies of the cameras should be stored in
// This file could be used as the capability file of this camera
// for the cedar::dev::sensors::visual::CameraGrabber class
#define CAP_FILE_NAME(GUID) ("camera_"+boost::lexical_cast<std::string>(GUID)+".capabilities")

using namespace cedar::dev::sensors::visual::grabbertools;

// ------------------------------------------------------------------------------------------------------------------
// anon. namespace for functions
namespace
{
  //print out the capabilities of a given feature
  void writeFeature(std::ostream& capFile, std::string& featureName, dc1394feature_info_t& featureInfo )
  {
    std::cout << "write feature: " << featureName << std::endl;
    switch (featureInfo.id)
    {
      //special values for triggers
      case DC1394_FEATURE_TRIGGER:
      case DC1394_FEATURE_TRIGGER_DELAY:
        //currently not supported by cedar/opencv
        capFile << featureName << "_is_supported"<< " = " << false << ";\n";
        capFile << featureName << "_is_readable"<< " = " << false << ";\n";
        break;
      
      //@todo:
      //case white_balance: red and blue eintragen!! 
      // jetzt: BLUE wird eingetragen, red nicht. 
      //  WHITE_BALANCE_RED_V
      
      default:
        
        capFile << featureName << "_is_supported"<< " = " << (bool)featureInfo.available << ";\n";
        capFile << featureName << "_is_readable"<< " = " << (bool)featureInfo.readout_capable << ";\n";
        capFile << featureName << "_min_value"<< " = " << featureInfo.min << ";\n";
        capFile << featureName << "_max_value" << " = " << featureInfo.max << ";\n";
        capFile << featureName << "_is_on_off_capable"<< " = " << (bool)featureInfo.on_off_capable << ";\n";
        capFile << featureName << "_is_absolute_capable"<< " = " << (bool)featureInfo.absolute_capable << ";\n";

        //check supported modes
        dc1394feature_modes_t& supported_modes = featureInfo.modes;
        
        bool mode_auto = false;
        bool mode_manual = false;
        bool mode_one_push = false;
        
        for(unsigned int i=0; i< supported_modes.num; i++)
        {
          switch (supported_modes.modes[i])
          {
            case DC1394_FEATURE_MODE_MANUAL: 
                          mode_manual = true; 
                          break;
            
            case DC1394_FEATURE_MODE_AUTO: 
                          mode_auto = true; 
                          break;

            case DC1394_FEATURE_MODE_ONE_PUSH_AUTO: 
                          mode_one_push = true; 
                          break;
            default: 
                          continue;
          }
        }
        
        capFile << featureName << "_is_manual_capable"<< " = " << mode_manual << ";\n";
        capFile << featureName << "_is_one_push_capable"<< " = " << mode_one_push << ";\n";
        capFile << featureName << "_is_auto_capable"<< " = " << mode_auto << ";\n";
      }
    }
} //end anon namespace

// ------------------------------------------------------------------------------------------------------------------
int main(int , char**)
{

  //create firewire interface
  cedar::dev::sensors::visual::grabbertools::FirewireInterface fw_interface;
  
  if (fw_interface.getNumCams() < 1)
  {
    std::cout << "No cameras found\n";
    fw_interface.cleanUp();
    return -1;
  }


  for (unsigned int camera_nr = 0; camera_nr < fw_interface.getNumCams(); camera_nr++)
  {
    //Get GUID of camera
    uint64_t guid;
    guid = fw_interface.getCamGuid(camera_nr);
    unsigned int guid_int = (unsigned int)(guid&0x00000000FFFFFFFF);  //only lower 32bit

    if (guid == 0)
    {
      std::cout << "Couldn't find camera!" << std::endl;
      return -1;
    }

    //open the cam
    fw_interface.openCamera(guid);

    //open the capabilities file for writing    
    std::ofstream cap_file;
    std::string file_name = CAP_FILE_NAME(guid_int);
    try
    {
      //open file
      cap_file.open(file_name.c_str());
      if (! cap_file.is_open())
      {
        throw;
      }
      //show bool values as true/false instead of 0/1
      cap_file << std::boolalpha;
      cap_file << "GUID = " << guid_int << ";\n";
      
      //--------------------------------------------------------------
      //display Camera infos
      std::cout << "\nCamera "<< camera_nr << ": GUID " << guid_int << std::endl;
      std::cout << "Write camera capabilities to \""<< file_name <<"\"\n";
      
      //read all features from cam
      dc1394featureset_t cam_features = fw_interface.getCamFeatures();
      
      for (int i = 0 ; i < DC1394_FEATURE_NUM; i++)
      {
        std::string feature_name = fw_interface.DC1394FeatureToCameraGrabberPropertyString(cam_features.feature[i].id);

        //check if cedar/opencv support this feature
        if (feature_name != "")
        {
           writeFeature(cap_file, feature_name, cam_features.feature[i]);
        }
      }
      std::cout << "done.\n";
      

    }
    catch (...)
    {
    }

    //cleanup
    cap_file.close();
    fw_interface.closeCamera();
  }

	return 0;
}
