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

    File:        tools.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Math tools

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "auxiliaries/math/tools.h"
#include "auxiliaries/math/constants.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>

using namespace std;

double cedar::aux::math::normalizeAngle(double value)
{
  while (value <= -cedar::aux::math::pi)
  {
    value += 2 * cedar::aux::math::pi;
  }
  while (value > cedar::aux::math::pi)
  {
    value -= 2 * cedar::aux::math::pi;
  }
  return value;
}

double cedar::aux::math::max(const cv::Mat matrix)
{
  double max;
  cv::minMaxLoc(matrix, 0 , &max);
  return max;
}

double cedar::aux::math::min(const cv::Mat matrix)
{
  double min;
  cv::minMaxLoc(matrix, &min );
  return min;
}

void cedar::aux::math::write(cv::Mat matrix)
{
  switch (matrix.type())
  {
  case CV_8U:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j = 0; j < matrix.cols; j++)
      {
        if (IsZero(matrix.at<unsigned char>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<unsigned char>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_8S:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j = 0; j < matrix.cols; j++)
      {
        if (IsZero(matrix.at<char>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<char>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_16U:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j = 0; j < matrix.cols; j++)
        {
          if (IsZero(matrix.at<unsigned short int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<unsigned short int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_16S:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j = 0; j < matrix.cols; j++)
        {
          if (IsZero(matrix.at<short int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<short int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_32S:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j=0; j<matrix.cols; j++)
        {
          if (IsZero(matrix.at<int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_32F:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j=0; j<matrix.cols; j++)
      {
        if (IsZero(matrix.at<float>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<float>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_64F:
  for(int i = 0; i < matrix.rows; i++)
  {
    for (int j=0; j<matrix.cols; j++)
    {
      if (IsZero(matrix.at<double>(i, j)))
      {
        cout << "0" << " ";
      }
      else
      {
        cout << matrix.at<double>(i, j) << " ";
      }
    }
    cout << "\n";
  }
  default:
    break;
  }
  cout << "\n";
}
