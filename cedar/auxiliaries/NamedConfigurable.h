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

    File:        NamedConfigurable.h

    Maintainer:  Oliver Lomp,
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NAMED_CONFIGURABLE_H
#define CEDAR_AUX_NAMED_CONFIGURABLE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/NamedConfigurable.fwd.h"

// SYSTEM INCLUDES


/*!@brief This is a Configurable with a name.
 *
 * This class is provided for convenience, because many classes derived from cedar::aux::Configurable use a name.
 */
class cedar::aux::NamedConfigurable : virtual public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NamedConfigurable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set the name of this NamedConfigurable
  void setName(const std::string& name);
  //!@brief get the name of this NamedConfigurable
  const std::string& getName() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

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
  //!@brief the name of this parameter
  StringParameterPtr _mName;

private:
  // none yet

}; // class cedar::aux::NamedConfigurable

#endif // CEDAR_AUX_NAMED_CONFIGURABLE_H
