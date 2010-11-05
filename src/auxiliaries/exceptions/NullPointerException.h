/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        NullPointerException.h

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 01 20

 ----- Description: Header for the \em cedar::aux::exc::NullPointerException class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_EXC_NULL_POINTER_EXCEPTION_H
#define CEDAR_AUX_EXC_NULL_POINTER_EXCEPTION_H

// LOCAL INCLUDES
#include "Namespace.h"
#include "ExceptionBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief A null pointer exception.
 *
 * Incidentally, this exception should be thrown when a null-pointer is accessed.
 */
class cedar::aux::exc::NullPointerException : public cedar::aux::exc::ExceptionBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NullPointerException(void);

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

}; // class cedar::aux::exc::NullPointerException

#endif // CEDAR_AUX_EXC_NULL_POINTER_EXCEPTION_H
