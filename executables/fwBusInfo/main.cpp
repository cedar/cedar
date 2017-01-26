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

    File:        main.cpp (fwBusInfo)

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 02 3

    Description: Short program to list all available cameras on the firewire bus


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

/*! A short program to search and list all available cameras with their GUID's on the firewire bus
 */
int main(int, char *argv[])
{
  std::cout << argv[0] << " is a program to list all available cameras on the firewire bus\n";

  //create firewire interface
  cedar::dev::sensors::camera::LibDcBase fw_interface;

  if (fw_interface.getNumCams() < 1)
  {
    std::cout << "No cameras found\n";
    fw_interface.cleanUp();
    return -1;
  }

  for (unsigned int camera_nr = 0; camera_nr < fw_interface.getNumCams(); camera_nr++)
  {
    uint64_t guid;
    guid = fw_interface.getCamGuid(camera_nr);
    unsigned int guid_int = static_cast<unsigned int>(guid&0x00000000FFFFFFFF);  //only lower 32bit

    std::cout << "bus-id: " << camera_nr << " : "
              << "guid: " << std::dec << guid_int
              << " (0x" << std::hex << guid_int << ")"
              << std::endl;
  }

  //open camera to check if cam avail.

  return 0;
}

#endif //CEDAR_USE_LIB_DC1394

