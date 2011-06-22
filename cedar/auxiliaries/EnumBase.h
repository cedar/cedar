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

    File:        EnumBase.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description: Base class for enums.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_BASE_H
#define CEDAR_AUX_ENUM_BASE_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/Enum.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>

//!\todo document EnumBase class
template <class T>
class EnumBase
{
  public:


    EnumBase(const std::string& prefix = "")
    :
    mUndefined (Enum::UNDEFINED, prefix + "UNDEFINED")
    {
      T::construct();
      this->def(mUndefined);
    }

    virtual ~EnumBase()
    {
    }

    void def(const Enum& rEnum)
    {
      //! @todo check for duplicate values
      this->mEnumFromId[rEnum.id()] = rEnum;
      this->mEnumFromString[rEnum.name()] = rEnum;
      this->mEnumList.push_back(rEnum);
    }

    const Enum& get(EnumId id) const
    {
      std::map<EnumId, Enum>::const_iterator it;
      it = this->mEnumFromId.find(id);
      if (it != this->mEnumFromId.end())
      {
        return it->second;
      }
      else
      {
        return this->mUndefined;
      }
    }

    const Enum& get(const std::string& id) const
    {
      std::map<std::string, Enum>::const_iterator it;
      it = this->mEnumFromString.find(id);
      if (it != this->mEnumFromString.end())
      {
        return it->second;
      }
      else
      {
        return this->mUndefined;
      }
    }

    const std::vector<Enum>& list() const
    {
      return this->mEnumList;
    }

  private:
    std::map<EnumId, Enum> mEnumFromId;
    std::map<std::string, Enum> mEnumFromString;
    std::vector<Enum> mEnumList;
    Enum mUndefined;
};

#endif // CEDAR_AUX_ENUM_BASE_H
