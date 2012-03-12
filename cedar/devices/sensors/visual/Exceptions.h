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
#include "cedar/auxiliaries/exceptions/ExceptionBase.h"


//------------------------------------------------------------------------
// namespace for exceptions
//------------------------------------------------------------------------
namespace cedar
{
  namespace aux
  {
    //! @brief Namespace for all exception classes.
    namespace exc
    {

      //! @brief An exception for errors on recording
      class GrabberRecordingException;

      //! @brief An exception for errors on saving a snapshot
      class GrabberSnapshotException;

      //! @brief An exception for errors on grabbing
      class GrabberGrabException;
    }
  }
}



//--------------------------------------------------------------------------------------------------------------------
// exceptions
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/*!@brief Exception for errors while saving a snapshots
 * \remarks
 *    This exception would be thrown when an error occurs while writing a snapshot to disk
 */
class cedar::aux::exc::GrabberSnapshotException
:
public cedar::aux::exc::ExceptionBase
{
public:
  //!@brief The standard constructor.
  GrabberSnapshotException(void);
};


//--------------------------------------------------------------------------------------------------------------------

/*!@brief Exception for errors while recording a stream
 * \remarks
 *    This exception would be thrown when an error occurs during recording.
 */
class cedar::aux::exc::GrabberRecordingException
:
public cedar::aux::exc::ExceptionBase
{
public:
  //!@brief The standard constructor.
  GrabberRecordingException(void);
};


//--------------------------------------------------------------------------------------------------------------------

/*!@brief Exception for errors while recording a stream
 * \remarks
 *    This exception would be thrown when an error occurs during recording.
 * \note
 *    This is a critical exception. The grabber-cleanup is performed right before this exception is thrown.<br>
 *    In this case, it is not possible to work any longer with this grabber.
 */
class cedar::aux::exc::GrabberGrabException
:
public cedar::aux::exc::ExceptionBase
{
public:
  //!@brief The standard constructor.
  GrabberGrabException(void);
};

#endif /* CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_H */
