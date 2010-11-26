/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Base.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 12

 ----- Description: Header for the @em cedar::aux::Base class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::aux::Base::Base()
{
}

//! destructor
cedar::aux::Base::~Base()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@brief Returns the name of the object.
//!@return Name of the object.
const std::string& cedar::aux::Base::getName(void) const
{
  return _mName;
}

//!@brief Sets the name of the object to the given name.
//!@param name New name of the object.
void cedar::aux::Base::setName(const std::string& rName)
{
  _mName = rName;
}
