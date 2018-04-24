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

    File:        sigmoids.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES

double cedar::aux::math::sigmoidExp(const double x, const double beta, const double threshold)
{
  return 1 / (1 + exp(-beta * (x - threshold)));
}

double cedar::aux::math::sigmoidAbs(const double x, const double beta, const double threshold)
{
  return 0.5 * (1. + beta * (x - threshold) / (1. + beta * fabs(x - threshold)));
}

double cedar::aux::math::sigmoidHeaviside(const double x, const double threshold)
{
  if (x < threshold)
    return 0.0;
  return 1.0;
}

double cedar::aux::math::sigmoidSemiLinear(double value, double threshold, double gain)
{
  if (value > threshold)
  {
    return threshold + gain * (value - threshold);
  }
  else
  {
    return threshold;
  }
}

template<typename T>
cv::Mat cedar::aux::math::sigmoid(const cv::Mat& mat, const double beta, const double threshold)
{
  cv::Mat result = mat.clone();
  for (int col = 0; col < mat.cols; col++)
  {
    for (int row = 0; row < mat.rows; row++)
    {
      result.at<T>(row,col) = sigmoidExp(mat.at<T>(row,col),beta,threshold);
    }
  }
  return result;
}
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sigmoid<double>(const cv::Mat&, const double, const double);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sigmoid<float>(const cv::Mat&, const double, const double);

//same as cv::mat sigmoidAbs but does not create new memory for result
template<typename T>
void cedar::aux::math::sigmoid(const cv::Mat& mat, cv::Mat& result, const double beta, const double threshold)
{
  for (int col = 0; col < mat.cols; col++)
  {
    for (int row = 0; row < mat.rows; row++)
    {
      result.at<T>(row,col) = sigmoidExp(mat.at<T>(row,col),beta,threshold);
    }
  }
}
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sigmoid<double>(const cv::Mat&, cv::Mat&, const double, const double);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sigmoid<float>(const cv::Mat&, cv::Mat&, const double, const double);

cv::Mat cedar::aux::math::sigmoidAbs(const cv::Mat& mat, const double beta, const double threshold)
{
  return 0.5 * (1.0 + beta * (mat - threshold) / (1.0 + beta * cv::abs(mat - threshold)));
}

template<typename T>
cv::Mat cedar::aux::math::sigmoidAbs(const cv::Mat& mat, const double beta, const double threshold)
{
  return sigmoidAbs(mat, beta, threshold);
}

template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sigmoidAbs<double>(const cv::Mat&, const double, const double);
template CEDAR_AUX_LIB_EXPORT cv::Mat cedar::aux::math::sigmoidAbs<float>(const cv::Mat&, const double, const double);

//same as cv::mat sigmoidAbs but does not create new memory for result
void cedar::aux::math::sigmoidAbs(const cv::Mat& mat, cv::Mat& result, const double beta, const double threshold)
{
  result = sigmoidAbs(mat, beta, threshold);
}

//same as cv::mat sigmoidAbs but does not create new memory for result
template<typename T>
void cedar::aux::math::sigmoidAbs(const cv::Mat& mat, cv::Mat& result, const double beta, const double threshold)
{
  result = sigmoidAbs(mat, beta, threshold);
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sigmoidAbs<double>(const cv::Mat&, cv::Mat&, const double, const double);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::sigmoidAbs<float>(const cv::Mat&, cv::Mat&, const double, const double);

std::vector<double> cedar::aux::math::sigmoid(const std::vector<double>& x, const double beta, const double threshold)
{
  std::vector<double> buffer;
  buffer.resize(x.size());
  for (unsigned int i = 0; i < x.size(); i++)
  {
    buffer[i] = sigmoidExp(x[i], beta, threshold);
  }
  return buffer;
}

double cedar::aux::math::sigmoidInterval(const double value, const double t1, const double t2, const bool decreasing)
{
  if (decreasing)
  {
    return 1 - sigmoidInterval(value, t1, t2);
  }
  if (value < t1)
  {
    return 0;
  }
  if (value > t2)
  {
    return 1;
  }
  else
  {
    return 0.5 * (-cos((value - t1) / (t2 - t1) * cedar::aux::math::pi) + 1);
  }
}
