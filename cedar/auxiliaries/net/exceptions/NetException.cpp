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

    File:        NetException

    Maintainer:  
    Email:       
    Date:        2011 06 16

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/net/exceptions/NetException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! Constructor
cedar::aux::exc::NetMissingRessourceException::NetMissingRessourceException()
{
  // Sets the type name.
  //@todo: GH commented on merge
  //this->mType = "YARP missing ressource";
}

//! Constructor
cedar::aux::exc::NetUnexpectedDataException::NetUnexpectedDataException()
{
  // Sets the type name.
  //@todo: GH commented on merge
  //this->mType = "YARP unexpected data";
}

//! Constructor
cedar::aux::exc::NetWaitingForWriterException::NetWaitingForWriterException()
{
  // Sets the type name.
  //@todo: GH commented on merge
  //this->mType = "YARP cannot connect ports; waiting for writer";
}

