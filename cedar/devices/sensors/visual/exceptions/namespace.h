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

    File:        namespace.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Namespace  for the @em cedar::devices::sensors::visual.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_NAMESPACE_H
#define CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_NAMESPACE_H



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

    //class GrabberGrabException;
    }
  }
}


#endif /* CEDAR_DEV_SENSORS_VISUAL_EXCEPTIONS_NAMESPACE_H */
