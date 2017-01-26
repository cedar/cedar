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

    File:        Dimensions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_DIMENSIONS_H
#define CEDAR_AUX_ANNOTATION_DIMENSIONS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/Annotation.h"
#include "cedar/auxiliaries/Cloneable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/Dimensions.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <vector>

/*!@brief An annotation that provides labels and descriptions for dimensions.
 *
 *        Exemplary usage of this annotation includes giving meaning to the dimensions of a matrix. For example, a
 *        two-dimensional matrix might represent space in its rows and color in its columns.
 */
class cedar::aux::annotation::Dimensions
:
public cedar::aux::annotation::Annotation,
public cedar::aux::Cloneable<cedar::aux::annotation::Dimensions, cedar::aux::annotation::Annotation>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct Dimension
  {
    std::string mLabel;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Dimensions(unsigned int numberOfDimensions);

  //! the copy constructor
  Dimensions(const cedar::aux::annotation::Dimensions& copy);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the label for the given dimension.
  const std::string& getLabel(unsigned int dimension) const;

  //! Sets the label of the given dimension.
  void setLabel(unsigned int dimension, const std::string& label);

  //! Returns the number of entries in the dimensions.
  inline size_t getDimensionality() const
  {
    return this->mDimensions.size();
  }

  //! Generates the description.
  std::string getDescription() const;

  //! Returns true if sampling positions are given for the requested dimension.
  bool hasSamplingPositions(unsigned int dimension) const;

  /*! Returns the sampling positions for the requested dimension.
   *
   *  Sampling positions specify for each position along an axis, what part of the space this sample lies in.
   *
   *  For example: say a 1d matrix has this annotation. Then this value indicates for each index of the matrix, that the
   *  value of that index, together with the sampling position for that index, specifies a pair
   *  (sampling position(index), matrix value(index)).
   */
  const std::vector<double>& getSamplingPositions(unsigned int dimension) const;

  //! Sets the sampling positions for the given dimension.
  void setSamplingPositions(unsigned int dimension, const std::vector<double>& positions);

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
  //! Labels corresponding to the dimensions
  std::vector<Dimension> mDimensions;

  std::vector< std::vector<double> > mDimensionSamplingPositions;

}; // class cedar::aux::annotation::Dimensions

#endif // CEDAR_AUX_ANNOTATION_DIMENSIONS_H

