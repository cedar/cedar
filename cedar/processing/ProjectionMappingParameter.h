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

    File:        ProjectionMappingParameter.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 21

    Description: Parameter type for the projection mapping.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H
#define CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/ProjectionMapping.fwd.h"
#include "cedar/processing/ProjectionMappingParameter.fwd.h"

// SYSTEM INCLUDES


/*!@brief Parameter type for the projection mapping.
 *
 * This class wraps the projection mapping type and supplies functions for using it as a parameter.
 */
class cedar::proc::ProjectionMappingParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMappingParameter(
                              cedar::aux::Configurable *pOwner,
                              const std::string& name
                            );

  //!@brief A constructor taking a default mapping.
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

  /*!@brief Read a configuration mapping from a configuration tree.
   *
   * @param root root of the configuration node holding the mapping information
   */
  void readFromNode(const cedar::aux::ConfigurationNode& root);

  /*!@brief Write the current mapping into a configuration tree.
   *
   * @param root root of the configuration node the mapping information will be written to
   */
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  /*!@brief Initialize the mapping with a given number of mappings.
   *
   * @param numberOfMappings number of initial mappings
   */
  void initialize(unsigned int numberOfMappings);

  /*!@brief Change the mapping from inputIndex to the new outputIndex.
   *
   * This method does bounds checking on both indices supplied.
   *
   * @param inputIndex index of input dimension
   * @param outputIndex index of the output dimension
   */
  void changeMapping(unsigned int inputIndex, unsigned int outputIndex);

  /*!@brief Configure the mapping to drop the dimension with the given input index.
   *
   * @param inputIndex index of the dimension to be dropped
   */
  void drop(unsigned int inputIndex);

  /*!@brief Inform the mapping about the output dimensionality of the projection.
   *
   * @param dimensionality the new output dimensionality of the projection
   */
  void setOutputDimensionality(unsigned int dimensionality);

  //!@brief Return the projection mapping.
  const cedar::proc::ProjectionMappingPtr& getValue() const;

  /*!@brief Create a default mapping.
   *
   * This method is empty.
   */
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
protected:
  // none yet
private:
  //! the actual mapping
  cedar::proc::ProjectionMappingPtr mValues;

}; // class cedar::proc::ProjectionMappingParameter

#endif // CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H
