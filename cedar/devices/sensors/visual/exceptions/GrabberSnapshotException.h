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

    File:        InitializationException.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ruhr-uni-bochum.de
    Date:        2011 08 01

    Description: Header for the @em cedar::aux::exc::GrabberRecordingException class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_EXC_GRABBER_SNAPSHOT_EXCEPTION_H
#define CEDAR_AUX_EXC_GRABBER_SNAPSHOT_EXCEPTION_H

// LOCAL INCLUDES
#include "namespace.h"
#include <auxiliaries/exceptions/ExceptionBase.h>

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Exception for errors while saving a snapshots
 *
 * This exception should be thrown when an error occurs during recording.
  */
class cedar::aux::exc::GrabberSnapshotException : public cedar::aux::exc::ExceptionBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GrabberSnapshotException(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

};

#endif // CEDAR_AUX_EXC_GRABBER_SNAPSHOT_EXCEPTION_H