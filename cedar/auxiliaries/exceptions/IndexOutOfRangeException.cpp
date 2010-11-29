/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        IndexOutOfRangeException.cpp

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 01 20

 ----- Description: Implementation of the @em cedar::aux::exc::NullPointerException class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "IndexOutOfRangeException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! Constructor
cedar::aux::exc::IndexOutOfRangeException::IndexOutOfRangeException()
{
  // Sets the type name.
  this->mType = "IndexOutOfRangeException";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
