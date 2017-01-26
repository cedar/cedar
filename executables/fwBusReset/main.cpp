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

    File:        main.cpp (fwBusReset)     

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 02 3

    Description: Short program to reset the first camera on the firewire bus and the bus itself


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


/*! A short program to search the bus for the first available camera, then resets it and the bus itself
 */
int main(int, char *argv[])
{
  std::cout << argv[0] << " resets the first camera on the firewire bus and the bus itself\n";

  //create firewire interface
  cedar::dev::sensors::camera::LibDcBase fw_interface;

  if (fw_interface.getNumCams() < 1)
  {
    std::cout << "No cameras found\n";
    fw_interface.cleanUp();
    return -1;
  }

  int camera_nr = 0;
  uint64_t guid;
  guid = fw_interface.getCamGuid(camera_nr);
  unsigned int guid_int = (unsigned int)(guid&0x00000000FFFFFFFF);  //only lower 32bit

  std::cout << "Found camera on bus-id: " << camera_nr << " : "
            << "guid: " << guid_int
            << " (0x" << std::hex << guid_int << ")\n"
            << "execute RESET commands"
            << std::endl;

  if (guid != 0)
  {
    //open the cam
    fw_interface.openCamera(guid);

    //Reset cam
    std::cout<<"\t- reset camera ... " << std::flush;
    if (fw_interface.resetCam())
    {
      std::cout<<"ok" << std::endl;
    }
    else
    {
      std::cout<<"error (firewire error string: " << fw_interface.getErrorString() << ")" <<std::endl;
    }

    //Reset bus
    std::cout<<"\t- reset firewire bus ... " << std::flush;
    if (fw_interface.resetFwBus())
    {
      std::cout<<"ok" << std::endl;
    }
    else
    {
      std::cout<<"error (firewire error string: " << fw_interface.getErrorString() << ")" <<std::endl;
    }


    //cleanup
    fw_interface.closeCamera();
  }
  return 0;
}

#endif // CEDAR_USE_LIB_DC1394
