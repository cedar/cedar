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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description: Parameter for a list of arbitrary length of dynamically allocatable, configurable objects. Ha!

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_LIST_PARAMETER_H
#define CEDAR_AUX_OBJECT_LIST_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES


/*!@brief Base class for a parameter that reads a list of configurable objects from a file.
 */
class cedar::aux::ObjectListParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectListParameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault)
  :
  cedar::aux::Parameter(pOwner, name, hasDefault)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief return the size of the vector
  virtual size_t size() const = 0;

  /*!@brief Returns the object with the given index.
   *
   * @todo There needs to be a const variant of this.
   */
  virtual cedar::aux::ConfigurablePtr configurableAt(size_t index) = 0;

  /*!@brief Fills a vector with the types that can be added to this object list
   */
  virtual void listTypes(std::vector<std::string>& types) const = 0;

  /*!@brief Returns the type id of the parameter with the given index.
   */
  virtual const std::string& getTypeOfObject(size_t index) const = 0;

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

}; // class cedar::aux::ObjectListParameter

#endif // CEDAR_AUX_OBJECT_LIST_PARAMETER_H

