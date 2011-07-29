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

    File:        DataSlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DATA_SLOT_H
#define CEDAR_PROC_DATA_SLOT_H

// LOCAL INCLUDES
#include "processing/namespace.h"
#include "processing/DataRole.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::DataSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Enum describing the validity of the data connected to this slot.
   */
  enum VALIDITY
  {
    //! The data is valid.
    VALIDITY_VALID,
    //! The data may not be valid, but the step using it can be computed nonetheless.
    VALIDITY_WARNING,
    //! The data is erroneous, computing the corresponding step may explode things.
    VALIDITY_ERROR,
    //! The validity is unknown and needs to be determined before execution.
    VALIDITY_UNKNOWN
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataSlot(cedar::proc::DataRole::Id role, const std::string& name, bool isMandatory = true);

  //!@brief Destructor
  virtual ~DataSlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::aux::DataPtr getData();
  cedar::aux::ConstDataPtr getData() const;

  void setData(cedar::aux::DataPtr data);

  bool isMandatory() const;

  VALIDITY getValidlity() const;
  void setValidity(VALIDITY validity);

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
  cedar::aux::DataPtr mData;
  bool mMandatory;
  VALIDITY mValidity;
  std::string mName;
  cedar::proc::DataRole::Id mRole;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::DataSlot

#endif // CEDAR_PROC_DATA_SLOT_H

