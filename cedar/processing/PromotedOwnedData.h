/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        PromotedOwnedData.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 01 31

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROMOTED_OWNED_DATA_H
#define CEDAR_PROC_PROMOTED_OWNED_DATA_H

// CEDAR INCLUDES
#include "cedar/processing/OwnedData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/PromotedOwnedData.fwd.h"

// SYSTEM INCLUDES
#include <string>

/*!@brief A slot for data that is promoted to an outer network.
 */
class cedar::proc::PromotedOwnedData : public cedar::proc::OwnedData
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PromotedOwnedData
  (
    cedar::proc::DataSlotPtr promotedSlot,
    cedar::proc::Connectable* pParent
  );

  //!@brief Destructor
  ~PromotedOwnedData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::aux::DataPtr getData();

  cedar::aux::ConstDataPtr getData() const;

  //!@brief get the current validity of the promoted slot
  VALIDITY getValidity() const;

  //!@brief set the current validity at the promoted slot
  void setValidity(VALIDITY validity);

  //! get the full path of slot promotion as dot-concatenated string
  std::string getPromotionPath() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void setData(cedar::aux::DataPtr data);

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
  cedar::proc::DataSlotPtr mDataSlot;
}; // class cedar::proc::PromotedOwnedData

#endif // CEDAR_PROC_PROMOTED_OWNED_DATA_H

