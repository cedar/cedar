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

    File:        Element.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_ELEMENT_H
#define CEDAR_PROC_ELEMENT_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/Configurable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Base class for Elements in a processing architecture.
 */
class cedar::proc::Element : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Element();

  //!@brief Destructor
  virtual ~Element();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Set the name of this element.
  void setName(const std::string& name);

  //!@brief Get the name of this element.
  const std::string& getName() const;

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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The name that uniquely identifies the element within its own module.
  cedar::aux::StringParameterPtr _mName;

private:
  // none yet

}; // class cedar::proc::Element

#endif // CEDAR_PROC_ELEMENT_H
