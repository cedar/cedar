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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 29

    Description: Header that holds defines for the whole cedar project.

                 ***************************************************************************
                 * Please consider _VERY CAREFULLY_, whether a (preprocessor) define/macro *
                 * is the best way to go. In a lot of cases it can be replaced by a static *
                 * const member or a template function!                                    *
                 ***************************************************************************

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_DEFINES_H
#define CEDAR_AUX_DEFINES_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/foreach.hpp>


//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

// foreach loop from boost
#define cedar_foreach BOOST_FOREACH



#endif // CEDAR_AUX_DEFINES_H
