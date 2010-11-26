/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ComponentNotAvailableException.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 08

 ----- Description: Header for the @em cedar::dev::robot::ComponentNotAvailableException class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "ComponentNotAvailableException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
ComponentNotAvailableException::ComponentNotAvailableException()
{
  // set the type name
  this->mType = "ComponentNotAvailableException";
}
