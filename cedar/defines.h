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


#ifndef CEDAR_DEFINES_H
#define CEDAR_DEFINES_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

// foreach loop from boost
#define cedar_foreach BOOST_FOREACH


//----------------------------------------------------------------------------------------------------------------------
// macros
//----------------------------------------------------------------------------------------------------------------------


/*! @def     CEDAR_GENERATE_POINTER_TYPES(CLASS_NAME)
 *
 *  @brief   Automatically declares the smart pointer types  of a class within a namespace block.
 *
 *           Calling namespace foo { CEDAR_GENERATE_POINTER_TYPES(Bar); } expands to
 *           namespace foo
 *           {
 *             typedef boost::shared_ptr<Bar> BarPtr;
 *             typedef boost::shared_ptr<const Bar> ConstBarPtr;
 *           }
 */
#define CEDAR_GENERATE_POINTER_TYPES(CLASS_NAME) \
  typedef boost::shared_ptr<CLASS_NAME> CLASS_NAME ## Ptr; \
  typedef boost::shared_ptr<const CLASS_NAME> Const ## CLASS_NAME ## Ptr; \
  typedef boost::weak_ptr<CLASS_NAME> CLASS_NAME ## WeakPtr; \
  typedef boost::weak_ptr<const CLASS_NAME> Const ## CLASS_NAME ## WeakPtr;

#define CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(CLASS_NAME) \
  typedef boost::intrusive_ptr<CLASS_NAME> CLASS_NAME ## Ptr; \
  typedef boost::intrusive_ptr<const CLASS_NAME> Const ## CLASS_NAME ## Ptr;

/*! @def     CEDAR_DECLARE_CLASS(CLASS_NAME)
 *
 *  @brief   Automatically declares a class and its corresponding smart pointer types within a namespace block.
 *
 *           Calling namespace foo { CEDAR_DECLARE_CLASS(Bar); } expands to
 *           namespace foo
 *           {
 *             class Bar;
 *             typedef boost::shared_ptr<Bar> BarPtr;
 *             typedef boost::shared_ptr<const Bar> ConstBarPtr;
 *           }
 */

#define CEDAR_DECLARE_CLASS(CLASS_NAME) \
  class CLASS_NAME; \
  CEDAR_GENERATE_POINTER_TYPES(CLASS_NAME)

#define CEDAR_DECLARE_CLASS_INTRUSIVE(CLASS_NAME) \
  class CLASS_NAME; \
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(CLASS_NAME)

#define CEDAR_DECLARE_CLASS_PREFIXED(PREFIX, CLASS_NAME) \
  class PREFIX CLASS_NAME; \
  CEDAR_GENERATE_POINTER_TYPES(CLASS_NAME)

#define CEDAR_DECLARE_CLASS_INTRUSIVE_PREFIXED(PREFIX, CLASS_NAME) \
  class PREFIX CLASS_NAME; \
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(CLASS_NAME)


#ifdef MSVC
#define CEDAR_DECLARE_DEPRECATED(x) __declspec(deprecated) x
#elif defined GCC
#define CEDAR_DECLARE_DEPRECATED(x) x __attribute__ ((deprecated))
#else
#error CEDAR_DECLARE_DEPRECATED is not implemented for this compiler.
#endif




#endif // CEDAR_DEFINES_H
