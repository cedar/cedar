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

    File:        BadConnectionException.h

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2010 12 08

    Description: Header for the @em cedar::aux::exc::BadConnectionException class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_EXC_BAD_CONNECTION_EXCEPTION_H
#define CEDAR_AUX_EXC_BAD_CONNECTION_EXCEPTION_H

#endif  //CEDAR_AUX_EXC_BAD_CONNECTION_EXCEPTION_H

// LOCAL INCLUDES
#include "ExceptionBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//none yet


/*!@brief Abstraction thrown when a connection (i.e. a network connection) is too bad to continue
 *
 */
class cedar::aux::exc::BadConnectionException : public cedar::aux::exc::ExceptionBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  BadConnectionException(void);

  /*!@brief The destructor.
   *
   * @remarks The destructor may not throw any exception.
   */
  virtual ~BadConnectionException(void) throw ();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //none yet

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

}; //class BadConnectionException
