/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        defines.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 29

 ----- Description: Header that holds defines for the whole cedar project.

                    ***************************************************************************
                    * Please consider _VERY CAREFULLY_, whether a (preprocessor) define/macro *
                    * is the best way to go. In a lot of cases it can be replaced by a static *
                    * const member or a template function!                                    *
                    ***************************************************************************
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

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
