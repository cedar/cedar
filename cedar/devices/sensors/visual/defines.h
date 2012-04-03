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

    File:        defines.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Defines for @em cedar::devices::sensors::visual.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_DEFINES_H
#define CEDAR_DEV_SENSORS_VISUAL_DEFINES_H

//!@todo: debug-output to logger




//------------------------------------------------------------------------
// Enable this, if you want to see some informations on initialization
//------------------------------------------------------------------------
#define SHOW_INIT_INFORMATION_VIDEOGRABBER
#define SHOW_INIT_INFORMATION_NETGRABBER
#define SHOW_INIT_INFORMATION_PICTUREGRABBER
#define SHOW_INIT_INFORMATION_CAMERAGRABBER

//------------------------------------------------------------------------
// Enable some warning output during operation
//------------------------------------------------------------------------
#define ENABLE_GRABBER_WARNING_OUTPUT

//------------------------------------------------------------------------
// Enable class-debugging output
//------------------------------------------------------------------------

#define DEBUG_GRABBER_INTERFACE
//#define DEBUG_GRABBER_STEP

#define DEBUG_NETGRABBER
#define DEBUG_VIDEOGRABBER
#define DEBUG_PICTUREGRABBER
#define DEBUG_CAMERAGRABBER

#endif

