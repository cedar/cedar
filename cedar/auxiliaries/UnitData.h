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

    File:        UnitData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 04 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UNIT_DATA_H
#define CEDAR_AUX_UNIT_DATA_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/UnitData.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Superclass for all data classes that hold data with units attached to them.
 */
class cedar::aux::UnitData : public cedar::aux::Data
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UnitData();

  //!@brief Destructor
  virtual ~UnitData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the suffix of the stored unit, as a string.
   *
   * @remarks This must be implemented in all subclasses.
   */
  virtual std::string getSuffix() const
  {
    return "";
  }

  /*!@brief Returns the value of the unit as a double, given the suffix returned by getSuffix().
   *
   * For example, if this contains 0.5 s, this will return 0.5.
   *
   * @remarks This must be implemented in all subclasses.
   */
  virtual double doubleValueForSuffix() const
  {
    return 0.0;
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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::UnitData

#endif // CEDAR_AUX_UNIT_DATA_H

