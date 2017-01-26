/*======================================================================================================================

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CallOnScopeExit.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 18

    Description: Header file for the class cedar::aux::CallOnScopeExit.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CALL_ON_SCOPE_EXIT_H
#define CEDAR_AUX_CALL_ON_SCOPE_EXIT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/CallOnScopeExit.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/bind.hpp>
  #include <boost/function.hpp>
#endif // Q_MOC_RUN


/*!@brief Objects of this class call a user-determined function when they go out of scope.
 *
 *        To use this object, create an instance, either as a pointer or just a normal object. Pass a function via
 *        boost::bind. When the object gets destroyed, the function you provided will be called.
 *
 *
 *        For example, say you have the following method:
 *        @code
 *          void print_stuff()
 *          {
 *            std::cout << "stuff called!" << std::endl;
 *          }
 *        @endcode
 *
 *        If you want this function called at the end of another function, you can do this like this:
 *        @code
 *          void another_function(bool something)
 *          {
 *            cedar::aux::CallOnScopeExit caller(boost::bind(&print_stuff));
 *
 *            // do stuff
 *            if (something)
 *            {
 *              // here, print_stuff will be called
 *              return;
 *            }
 *
 *            // do more stuff
 *
 *            // print_stuff will be called here if this is reached
 *          }
 *        @endcode
 *        Note, that in the above example, print_stuff will be called only once.
 */
class cedar::aux::CallOnScopeExit
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CallOnScopeExit(const boost::function<void ()>& function);

  //!@brief Destructor
  virtual ~CallOnScopeExit();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Calls the function. It will then no longer be called when the object is destroyed, unless
    * cedar::aux::CallOnScopeExit::resetCall is called.
    */
  void callNow();

  /*! Resets the internal call variable, meaning, that the function will again be called when the object is destroyed.
   *
   * @see cedar::aux::CallOnScopeExit::callNow
   */
  void resetCall();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*! This needs to be called by derived classes if the function to be called on scope exit accesses members of the
   *  Derived classes. Otherwise, these classes will access invalid memory.
   */
  void preDestruct();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  boost::function<void ()> mFunctionToCall;

  bool mCall;

}; // class cedar::aux::CallOnScopeExit

#endif // CEDAR_AUX_CALL_ON_SCOPE_EXIT_H

