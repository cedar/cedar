/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        exceptions.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 25 Apr 2012 06:45:49 PM CEST

    Description: The exceptions of the net subdirectory

    Credits:

=============================================================================*/

#ifndef CEDAR_EXCEPTIONS_H
#define CEDAR_EXCEPTIONS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/exceptions.fwd.h"
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES

/*!@brief exception when there is something critically missing for net transport
 *
 */
class cedar::aux::net::NetMissingRessourceException : public cedar::aux::ExceptionBase
{
}; // class 


/*!@brief exception for when the data was corrupted OR we expected data and there was none
 *
 */
class cedar::aux::net::NetUnexpectedDataException : public cedar::aux::ExceptionBase
{
}; // class 

/*!@brief we expected data and there was none
 *
 * Derives from NetUnexpectedDataException for backwards-compatibility.
 */
class cedar::aux::net::NetNoNewDataException : public cedar::aux::net::NetUnexpectedDataException
{
}; // class 

/*!@brief exception for when the transmitted data is explicitly not handled
 *
 */
class cedar::aux::net::NetUnhandledDataException : public cedar::aux::net::NetUnexpectedDataException
{
}; // class 

/*!@brief exception for when the communication 'channel' was not (yet) set up
 *
 */
class cedar::aux::net::NetWaitingForWriterException : public cedar::aux::net::NetMissingRessourceException
{
}; // class 


#endif // CEDAR_XXX_XXX_H
