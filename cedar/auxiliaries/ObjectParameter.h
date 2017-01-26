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

    File:        ObjectParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description: Parameter for a dynamically allocatable, configurable object.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_PARAMETER_H
#define CEDAR_AUX_OBJECT_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ObjectParameter.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A parameter that can store objects of dynamically determined type.
 *
 * @remarks The objects cannot themselves have a parameter with the name "type".
 */
class cedar::aux::ObjectParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  ObjectParameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault)
  :
  cedar::aux::Parameter(pOwner, name, hasDefault)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Lists all the types available to be stored in this parameter.
   */
  virtual void listTypes(std::vector<std::string>& types) const = 0;

  /*!@brief   Sets the type of the object stored in the parameter.
   *
   * @remarks This will create a new object.
   */
  virtual void setType(const std::string& type) = 0;

  //!@brief Returns the type of the object currently stored in the parameter.
  virtual const std::string& getTypeId() const = 0;

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

}; // class cedar::aux::ObjectParameter

#endif // CEDAR_AUX_OBJECT_PARAMETER_H

