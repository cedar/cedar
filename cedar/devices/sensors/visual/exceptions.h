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

    File:        Exceptions.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header file for all new exceptions introduced in the @em cedar::dev::sensors::visual namespace.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_H
#define CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/devices/sensors/visual/namespace.h"

/*!@brief Exception when an error in the creation of a grabber occur*/
class cedar::dev::sensors::visual::CreateGrabberException : public cedar::aux::ExceptionBase
{
};


/*!@brief Exception for errors while saving a snapshots
 *
 *    This exception would be thrown when an error occurs while writing a snapshot to disk
 */
class cedar::dev::sensors::visual::GrabberSnapshotException : public cedar::aux::ExceptionBase
{
};


/*! @brief Exception for errors while recording a stream
 *
 *    This exception would be thrown when an error occurs during recording.
 */
class cedar::dev::sensors::visual::GrabberRecordingException : public cedar::aux::ExceptionBase
{
};


/*! @brief Exception for errors while recording a stream
 *
 *    This exception would be thrown when an error occurs during recording.
 *  @remarks
 *    This is a critical exception. The grabber-cleanup is performed right before this exception is thrown.<br>
 *    In this case, it is not possible to work any longer with this grabber.
 */
class cedar::dev::sensors::visual::GrabberGrabException : public cedar::aux::ExceptionBase
{
};


/*!@brief Exception when an invalid parameter (false range, NULL-Pointer, ...) is given */
class cedar::dev::sensors::visual::InvalidParameterException : public cedar::aux::ExceptionBase
{
};

#endif /* CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_H */
