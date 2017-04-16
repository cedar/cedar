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
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/math/tools.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <string>

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
  //!@brief Returns a string that contains the matrix body in CSV format.
  void serializeData(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const;
  
  //!@brief Returns a string that describes the matrix header in CSV format.
  void serializeHeader(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const;

  void serialize(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const;

  void deserialize(std::istream& stream, cedar::aux::SerializationFormat::Id mode);
  
  //!@brief creates a deep copy of this data
  cedar::aux::DataPtr clone() const;

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

  void copyValueFrom(cedar::aux::ConstDataPtr data)
  {
    if (ConstMatDataPtr mat_data_ptr = boost::dynamic_pointer_cast<ConstMatData>(data))
    {
      this->setData(mat_data_ptr->getData().clone());
    }
    else
    {
      CEDAR_THROW(cedar::aux::TypeMismatchException, "Cannot cast given data to matrix data.");
    }
  }

  //! Checks if the matrix is empty.
  bool isEmpty() const
  {
    return this->getData().empty();
  }

  //! Returns the value of the contained (two-dimensional) matrix at the given location
  template <typename ReturnT>
  ReturnT getValue(int row, int col) const
  {
    return cedar::aux::math::getMatrixEntry<ReturnT>(this->getData(), row, col);
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
