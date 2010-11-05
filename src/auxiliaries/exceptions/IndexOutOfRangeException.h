/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        IndexOutOfRangeException.h

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 03 08

 ----- Description: Header for the \em cedar::aux::exc::IndexOutOfRangeException class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_EXC_INDEX_OUT_OF_RANGE_EXCEPTION_H
#define CEDAR_AUX_EXC_INDEX_OUT_OF_RANGE_EXCEPTION_H

// LOCAL INCLUDES
#include "Namespace.h"
#include "ExceptionBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief A null pointer exception.
 *
 * Incidentally, this exception should be thrown when a null-pointer is accessed.
 */
class cedar::aux::exc::IndexOutOfRangeException : public cedar::aux::exc::ExceptionBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IndexOutOfRangeException(void);

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

}; // class cedar::aux::exc::IndexOutOfRangeException

#endif // CEDAR_AUX_EXC_INDEX_OUT_OF_RANGE_EXCEPTION_H

