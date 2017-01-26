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

    File:        MatrixIterator.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2011 11 04

    Description: Header for the @em cedar::aux::MatrixIterator class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATRIX_ITERATOR_H
#define CEDAR_AUX_MATRIX_ITERATOR_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatrixIterator.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <vector>


/*!@brief This is an iterator that allows easy iteration of all elements in an n-dimensional matrix.
 *
 * For example, say you have a cv::Mat my_matrix that you want to iterate. Then you can do so with the following code:
 * @code
 * cv::Mat my_matrix = ...
 * cedar::aux::MatrixIterator iter(my_matrix);
 * do
 * {
 *   my_matrix.at<float>(iter.getCurrentIndex()) = ...
 * }
 * while (iter.increment());
 * @endcode
 *
 * @remarks The iterator stores a reference to the matrix, which means that the iterator must be in the same scope as
 *          the matrix itself. Otherwise, hilarity ensues!
 */
class cedar::aux::MatrixIterator
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param matrix The matrix to iterate.
   */
  MatrixIterator(const cv::Mat& matrix);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns
  inline const int* getCurrentIndex() const
  {
    return &this->mIndex.front();
  }

  //!@brief return the n-dimensional index of the current iterator position
  inline const std::vector<int>& getCurrentIndexVector() const
  {
    return this->mIndex;
  }

  /*!@brief increment the matrix iterator
   * @returns False, if the iterator arrived at the end of the matrix, true otherwise.
   */
  inline bool increment()
  {
    mIndex.front() += 1;

    // check for overflows
    size_t d = 0;
    while (d < mIndex.size() - 1 && mIndex.at(d) >= static_cast<int>(this->mMatrixRef.size[d]))
    {
      mIndex.at(d) = 0;
      mIndex.at(d + 1) += 1;
      ++d;
    }

    return mIndex.back() < static_cast<int>(this->mMatrixRef.size[this->mMatrixRef.dims - 1]);
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
  //!@brief an n-dimensional index for the current iterator
  std::vector<int> mIndex;
  //!@brief a reference to a cv::Mat
  const cv::Mat& mMatrixRef;
}; // class cedar::aux::MatrixIterator

#endif // CEDAR_AUX_MATRIX_ITERATOR_H
