/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Tools.cpp

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: math tools

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Tools.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>

using namespace std;

double cedar::aux::math::normalizeAngle(double value)
{
  while (value <= -M_PI)
  {
    value += 2 * M_PI;
  }
  while (value > M_PI)
  {
    value -= 2 * M_PI;
  }
  return value;
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
