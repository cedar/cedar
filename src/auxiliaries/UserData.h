/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        UserData.h

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 03 02

 ----- Description: Header for the @em cedar::aux::UserData class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_USER_DATA_H
#define CEDAR_AUX_USER_DATA_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>


/*!@brief This class is the base class for passing user data to the cedar::aux::ConfigurationInterface.
 *
 * If you want to pass your own data to the addParameter functions in the cedar::aux::ConfigurationInterface, make a
 * class that inherits from cedar::aux::UserData and stores all your values. Later, when you read the data, you can
 * \em dynamic_cast it back to your class and access the values.
 */
class cedar::aux::UserData : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  UserData();
  //!@brief destructor
  virtual ~UserData(void) = 0;

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
  // none yet
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::UserData

#endif // CEDAR_AUX_USER_DATA_H

