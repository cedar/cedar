/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        UnitDataTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 04 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UNIT_DATA_TEMPLATE_H
#define CEDAR_AUX_UNIT_DATA_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/UnitData.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/UnitDataTemplate.fwd.h"

// SYSTEM INCLUDES
#include <boost/units/io.hpp>


/*!@todo describe.
 *
 * @todo describe more.
 */
template <typename T>
class cedar::aux::UnitDataTemplate : public cedar::aux::UnitData
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef T Quantity;
  typedef typename Quantity::unit_type Unit;

private:
  typedef cedar::aux::UnitDataTemplate<T> SelfType;
  CEDAR_GENERATE_POINTER_TYPES(SelfType);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UnitDataTemplate()
  {
  }

  //! Constructor that takes a quantity.
  UnitDataTemplate(const Quantity& quantity)
  {
    this->setData(quantity);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getSuffix() const
  {
    return boost::units::symbol_string(Unit());
  }

  double doubleValueForSuffix() const
  {
    return this->mData.value();
  }

  //!@brief returns the internal data
  Quantity& getData()
  {
    return this->mData;
  }

  //!@brief returns the internal data as const
  const Quantity& getData() const
  {
    return this->mData;
  }

  //!@brief sets the internal data to the given data/value
  void setData(const Quantity& data)
  {
    this->mData = data;
  }

  void copyValueFrom(cedar::aux::ConstDataPtr data)
  {
    if (ConstSelfTypePtr self_type_ptr = boost::dynamic_pointer_cast<ConstSelfType>(data))
    {
      this->setData(self_type_ptr->getData());
    }
    else
    {
      CEDAR_THROW(cedar::aux::TypeMismatchException, "Cannot cast to the appropriate type.");
    }
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
  //!@brief the internal data
  Quantity mData;

}; // class cedar::aux::UnitDataTemplate

#endif // CEDAR_AUX_UNIT_DATA_TEMPLATE_H

