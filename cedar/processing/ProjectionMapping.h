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

    File:        ProjectionMapping.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 18

    Description: Description of a mapping between dimensions.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAPPING_H
#define CEDAR_PROC_PROJECTION_MAPPING_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/ProjectionMapping.fwd.h"
#include "cedar/processing/ProjectionMappingParameter.fwd.h"

// SYSTEM INCLUDES
#include <map>
#include <vector>

/*!@brief Description of a mapping between dimensions.
 *
 * This class is used by the projection to store the mapping between the dimensions of two steps.
 */
class cedar::proc::ProjectionMapping
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::ProjectionMappingParameter;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef std::map<unsigned int, unsigned int> MappingType;
  typedef std::map<unsigned int, MappingType> MappingMap;
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
  unsigned int lookUp(unsigned int inputIndex) const;

  //!@brief Return the current number of mappings.
  unsigned int getNumberOfMappings() const;

  /*!@brief Test whether an input dimension is dropped.
   *
   * @param inputIndex index of the input dimension that is to be tested
   */
  bool isDropped(unsigned int inputIndex) const;

  /*!@brief Tests whether a mapping exists for a given input dimension.
   *
   * @param inputIndex index of the input dimension, whose mapping is to be checked for existence
   */
  bool mappingExists(unsigned int inputIndex) const;

  /*!@brief Set the output dimensionality of the mapping.
   *
   * @param dimensionality the new dimensionality of the output of the mapping
   */
  void setOutputDimensionality(unsigned int dimensionality);

  //!@brief Get the output dimensionality of the mapping.
  unsigned int getOutputDimensionality() const;

  //!@brief Return the validity of the mapping.
  VALIDITY getValidity() const;

  //!@brief Return an iterator to the first mapping.
  cedar::proc::ProjectionMapping::iterator begin();
  //!@brief Return a const iterator to the first mapping
  cedar::proc::ProjectionMapping::const_iterator begin() const;
  //!@brief Return an iterator to the end of the map (one after the last element).
  cedar::proc::ProjectionMapping::iterator end();
  //!@brief Return a const iterator to the end of the map (one after the last element).
  cedar::proc::ProjectionMapping::const_iterator end() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Update the state (clean or erroneous) of the mapping.
   *
   * When adding a new mapping, this method checks whether the new overall mapping still
   * makes sense in the given context and updates the state of the mapping accordingly.
   */
  void updateValidity();

  /*!@brief Add a mapping without any checks.
   *
   * This method is used by the associated parameter when loading a saved mapping from a
   * configuration file.
   *
   * @param inputIndex index of the input dimension
   * @param outputIndex index of the output dimension
   */
  void addMapping(unsigned int inputIndex, unsigned int outputIndex);

  /*!@brief Check whether the index of the output dimension is out of bounds.
   *
   * @param outputIndex the index of the output dimension
   */
  bool checkOutputIndex(unsigned int outputIndex) const;

  /*!@brief Check whether the index of the input dimension is out of bounds.
   *
   * @param inputIndex the index of the input dimension
   */
  bool checkInputIndex(unsigned int inputIndex) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! index used to represent dropping of a dimension
  static unsigned int msDropIndex;
  //! data structure containing the actual overall mapping
  MappingType mMapping;
  MappingMap mMappingsMap;
  //! output dimensionality of the mapping
  unsigned int mOutputDimensionality;
  //! validity of the mapping (whether it can be used in the given context)
  VALIDITY mValidity;
  std::vector<unsigned int> mDefaultValues;

}; // class cedar::proc::ProjectionMapping

#endif // CEDAR_PROC_PROJECTION_MAPPING_H
