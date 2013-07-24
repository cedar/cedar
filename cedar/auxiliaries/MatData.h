/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 12 09

    Description: This is a dummy header for the typedef MatData (which is actually a cedar::aux::DataTemplate<cv::Mat>).

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MAT_DATA_H
#define CEDAR_AUX_MAT_DATA_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/DataTemplate.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>

/*!@brief Data containing matrices.
 */
class cedar::aux::MatData : public cedar::aux::DataTemplate<cv::Mat>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::DataTemplate<cv::Mat> Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatData()
  {
  }

  //!@brief This constructor initializes the internal data to a value.
  MatData(const cv::Mat& value)
  :
  cedar::aux::DataTemplate<cv::Mat>(value)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getDescription() const;

  /*!@brief Returns the dimensionality of the matrix stored in this data.
   *
   * @remarks Calls cedar::aux::math::getDimensionalityOf(this->getData()) to determine the dimensionality.
   */
  unsigned int getDimensionality() const;

  //! Convenience method that returns the opencv-type of the stored matrix.
  inline int getCvType() const
  {
    return this->getData().type();
  }

  //! Checks if the matrix is empty.
  bool isEmpty() const
  {
    return this->getData().empty();
  }

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

}; // class cedar::aux::MatData

#endif // CEDAR_AUX_MAT_DATA_H
