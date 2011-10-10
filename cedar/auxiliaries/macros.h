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

    File:        macros.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description: Header for several macros.

    Credits:

======================================================================================================================*/

/*!@file  macros.h
 *
 * @brief File containing multiple global macros for cedar.
 */

#ifndef CEDAR_MACROS_H
#define CEDAR_MACROS_H

#define CEDAR_GENERATE_POINTER_TYPES(CLASS_NAME) \
  typedef boost::shared_ptr<CLASS_NAME> CLASS_NAME ## Ptr; \
  typedef boost::shared_ptr<const CLASS_NAME> Const ## CLASS_NAME ## Ptr;

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

#endif // CEDAR_MACROS_H

