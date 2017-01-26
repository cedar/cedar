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

    File:        Cloneable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 02

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CLONEABLE_H
#define CEDAR_AUX_CLONEABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Cloneable.fwd.h"

// SYSTEM INCLUDES

/*!@brief Base class for cloneables.
 */
template <class ReturnedT>
class cedar::aux::CloneableBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The type returned by the clone function.
  typedef ReturnedT ReturnedType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Virtual base destructor.
   */
  virtual ~CloneableBase()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns a clone of this object.
  virtual boost::shared_ptr<ReturnedType> clone() const = 0;
};

/*!@brief  A class that implements the clone function based on a class's copy constructor.
 *
 *         Any class that uses this interface can be cloned automatically. To use, simply inherit from this interface
 *         with the appropriate template arguments.
 *
 * @tparam ClonedT   The type that is being cloned.
 * @tparam ReturnedT The type that is returned; usually, the common base class in a hierarchy of cloneable objects.
 */
template <class ClonedT, class ReturnedT>
class cedar::aux::Cloneable : public virtual cedar::aux::CloneableBase<ReturnedT>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The type of the object being cloned.
  typedef ClonedT ClonedType;

  //! The type used to return the object.
  typedef ReturnedT ReturnedType;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Destructor.
  virtual ~Cloneable()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Clones the object using the copy constructor.
   */
  virtual boost::shared_ptr<ReturnedType> clone() const
  {
    return boost::shared_ptr<ReturnedType>(new ClonedType(*cedar::aux::asserted_cast<const ClonedT*>(this)));
  }

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
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::Cloneable

#endif // CEDAR_AUX_CLONEABLE_H

