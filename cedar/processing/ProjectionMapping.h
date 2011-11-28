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

    File:        ProjectionMapping.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAP_H
#define CEDAR_PROC_PROJECTION_MAP_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::ProjectionMapping
{
  friend class cedar::proc::ProjectionMappingParameter;

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------

  typedef std::map<unsigned int, unsigned int> MappingType;
  typedef MappingType::iterator iterator;
  typedef MappingType::const_iterator const_iterator;

public:
  //! Enum describing the validity of the mapping.
  enum VALIDITY
  {
    //! The data is valid.
    VALIDITY_VALID,
    //! The data may not be valid, but the step using it can be computed nonetheless.
    VALIDITY_WARNING,
    //! The data is erroneous, computing the corresponding step may explode things.
    VALIDITY_ERROR
  };

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMapping();

  //!@brief Destructor
  ~ProjectionMapping();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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
  /*!@brief Initialize or reinitialize the mapping with the given number of mappings,
   * where initially all input dimensions are dropped.
   *
   * @param numberOfMappings the number of initial mappings
   */
  void initialize(unsigned int numberOfMappings);

  /*!@brief Add or change a mapping from the input dimension with index inputIndex
   * to the output dimension with index outputIndex.
   *
   * @param inputIndex index of the input dimension to be mapped
   * @param outputIndex index of the output dimension the input dimension is to be mapped on
   */
  void changeMapping(unsigned int inputIndex, unsigned int outputIndex);

  /*!@brief Drop an input dimension.
   *
   * When compressing a higher dimensional input, the excess dimensions must be dropped.
   * Dropping dimensions does not make sense when the projection expands an input.
   *
   * @param inputIndex the index of the input dimension that is to be dropped
   */
  void drop(unsigned int inputIndex);

  /*!@brief Look up the index of an output dimension for a given input dimension.
   *
   * @param inputIndex index of the input dimension, whose corresponding output dimension is to be looked up
   */
  unsigned int lookUp(unsigned int inputIndex);

  //!@brief Return the current number of mappings.
  unsigned int getNumberOfMappings();

  /*!@brief Test whether an input dimension is dropped.
   *
   * @param inputIndex index of the input dimension that is to be tested
   */
  bool isDropped(unsigned int inputIndex);

  /*!@brief Tests whether a mapping exists for a given input dimension.
   *
   * @param inputIndex index of the input dimension, whose mapping is to be checked for existence
   */
  bool mappingExists(unsigned int inputIndex);

  /*!@brief Set the output dimensionality of the mapping.
   *
   * @param dimensionality the new dimensionality of the output of the mapping
   */
  void setOutputDimensionality(unsigned int dimensionality);

  //!@brief Get the output dimensionality of the mapping.
  unsigned int getOutputDimensionality();

  //!@brief Return the validity of the mapping.
  VALIDITY getValidity();

  //!@brief Return an iterator to the first mapping.
  cedar::proc::ProjectionMapping::iterator begin();
  //!@brief Return a const iterator to the first mapping
  cedar::proc::ProjectionMapping::const_iterator begin() const;
  //!@brief Return an iterator to the end of the map (one after the last element).
  cedar::proc::ProjectionMapping::iterator end();
  //!@brief Return a const iterator to the end of the map (one after the last element).
  cedar::proc::ProjectionMapping::const_iterator end() const;

protected:
  // none yet
private:
  /*!@brief Update the state (clean or erroneous) of the mapping.
   *
   * When adding a new mapping, this method checks whether the new overall mapping still
   * makes sense in the given context and updates the state of the mapping accordingly.
   */
  void updateValidity();

  //@todo write documentation
  void addMapping(unsigned int inputIndex, unsigned int outputIndex);

  /*!@brief Check whether the index of the output dimension is out of bounds.
   *
   * @param outputIndex the index of the output dimension
   */
  bool checkOutputIndex(unsigned int outputIndex);

  /*!@brief Check whether the index of the input dimension is out of bounds.
   *
   * @param inputIndex the index of the input dimension
   */
  bool checkInputIndex(unsigned int inputIndex);

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //! index used to represent dropping of a dimension
  static unsigned int msDropIndex;
  //! data structure containing the actual overall mapping
  MappingType mMapping;
  //! output dimensionality of the mapping
  unsigned int mOutputDimensionality;
  //! validity of the mapping (whether it can be used in the given context)
  VALIDITY mValidity;

}; // class cedar::proc::ProjectionMapping

#endif // CEDAR_PROC_PROJECTION_MAP_H
