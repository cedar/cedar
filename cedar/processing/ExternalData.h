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

    File:        ExternalData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXTERNAL_DATA_H
#define CEDAR_PROC_EXTERNAL_DATA_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/DataSlot.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief A slot for data that is owned by a Connectable.
 */
class cedar::proc::ExternalData : public cedar::proc::DataSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ExternalData(cedar::proc::DataRole::Id role, const std::string& name, bool isMandatory = true);

  //!@brief Destructor
  ~ExternalData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::aux::DataPtr getData();

  cedar::aux::ConstDataPtr getData() const;

  cedar::aux::DataPtr getData(unsigned int index);

  cedar::aux::ConstDataPtr getData(unsigned int index) const;

  void setData(cedar::aux::DataPtr data);

  void setData(cedar::aux::DataPtr data, unsigned int index);

  void addData(cedar::aux::DataPtr data);

  inline unsigned int getDataCount() const
  {
    return this->mData.size();
  }

  bool hasData(cedar::aux::ConstDataPtr data) const;

  void removeData(cedar::aux::ConstDataPtr data0);

  /*!
   * @remarks This function throws unless the role of this slot is input.
   */
  void setCollection(bool isCollection);

  bool isCollection() const;


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
  std::vector<cedar::aux::DataPtr> mData; //!@todo Make weak_ptr

  //!@brief Whether this slot can have multiple data items.
  bool mIsCollection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::ExternalData

#endif // CEDAR_PROC_EXTERNAL_DATA_H

