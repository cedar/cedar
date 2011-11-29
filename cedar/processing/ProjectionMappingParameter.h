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

    File:        ProjectionMapParameter.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H
#define CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"

// PROJECT INCLUDES
#include "cedar/processing/ProjectionMapping.h"

// SYSTEM INCLUDES
#include <climits>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::ProjectionMappingParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedef
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMappingParameter(
                              cedar::aux::Configurable *pOwner,
                              const std::string& name
                            );

  ProjectionMappingParameter(
                              cedar::aux::Configurable *pOwner,
                              const std::string& name,
                              const cedar::proc::ProjectionMappingPtr& defaults
                            );

  //!@brief Destructor
  ~ProjectionMappingParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setTo(const cedar::aux::ConfigurationNode& root);

  void putTo(cedar::aux::ConfigurationNode& root) const;

  void initialize(unsigned int numberOfMappings);

  void changeMapping(unsigned int inputIndex, unsigned int outputIndex);

  void drop(unsigned int inputIndex);

  void setOutputDimensionality(unsigned int dimensionality);

  const cedar::proc::ProjectionMappingPtr& getValue() const;

  void makeDefault();

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
  cedar::proc::ProjectionMappingPtr mValues;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::ProjectionMappingParameter

#endif // CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H

