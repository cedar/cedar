/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <fstream>
#include <QReadLocker>
#include <QReadWriteLock>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::aux::MatData::getDescription() const
{
  std::string description;
  description += Super::getDescription();
  description += "<hr />";

  this->lockForRead();
  const cv::Mat& mat = this->getData();
  if (mat.empty())
  {
    description += "Empty matrix.";
  }
  else
  {
    unsigned int dim = cedar::aux::math::getDimensionalityOf(mat);
    description += cedar::aux::toString(dim) + "-dimensional matrix";
    if (dim != 0)
    {
      description += "<br />size: ";
      if (dim == 1)
      {
        description += cedar::aux::toString(cedar::aux::math::get1DMatrixSize(mat));
        description += " (";
      }

      for (int i = 0; i < mat.dims; ++i)
      {
        if (i > 0)
        {
          description += " x ";
        }
        description += cedar::aux::toString(mat.size[i]);
      }

      if (dim == 1)
      {
        description += ")";
      }
    }
    description += "<br />";
    description += "type: " + cedar::aux::math::matrixTypeToString(mat) + "<br />";
    description += "channels: " + cedar::aux::toString(mat.channels());
  }

  this->unlock();

  return description;
}

void cedar::aux::MatData::serializeData(std::ostream& stream) const
{
  QReadLocker locker(this->mpLock);
  //creating index that addresses an element in the n dimensional Mat
  std::vector<int> index(mData.dims, 0);

  //iterate  as long the last dimension has exceeded
  while (index[mData.dims-1] < mData.size[mData.dims-1])
  {
    // get memory address of the element.
    uchar* element = mData.data;
    for (int i = 0; i < mData.dims; i++)
    {
      //Addresses an element of the Mat. See OpenCV Documentation.
      element += mData.step[i]*index[i];
    }
    //check data type
    for (int i = 0; i < mData.channels(); i++)
    {
      switch (mData.depth())
      {
        case CV_8U:
        {
          stream << static_cast<int>(element[i]) << ",";
          break;
        }
        case CV_8S:
        {
          stream  << static_cast<int>(reinterpret_cast<schar*>(element)[i]) << ",";
          break;
        }
        case CV_16U:
        {
          stream  << reinterpret_cast<unsigned short*>(element)[i] << ",";
          break;
        }
        case CV_16S:
        {
          stream  << reinterpret_cast<short*>(element)[i] << ",";
          break;
        }
        case CV_32S:
        {
          stream  << reinterpret_cast<int*>(element)[i] << ",";
          break;
        }
        case CV_32F:
        {
          stream  << reinterpret_cast<float*>(element)[i] << ",";
          break;
        }
        case CV_64F:
        {
          stream  << reinterpret_cast<double*>(element)[i] << ",";
          break;
        }
      }
    }

    //increase index
	  //!@todo This is slow in multiple ways; a faster approach would be to iterate over the linear memory with a linear index
    //!@todo Also, we have an iterator class for iterating over a 3d matrix
    index[0]++;
    for(int i =0; i < mData.dims-1; i++)
    {
      if(index[i] >= mData.size[i])
      {
        index[i]=0;
        index[i+1]++;
      }
    }
  }
}

void cedar::aux::MatData::serializeHeader(std::ostream& stream) const
{
  QReadLocker locker(this->mpLock);
  stream << "Mat" << ",";
  stream << cedar::aux::math::matrixTypeToString(mData) << ",";
  for(int i =0; i < mData.dims;i++)
  {
    stream << mData.size[i] << ",";
  }
}

cedar::aux::DataPtr cedar::aux::MatData::clone() const
{
  QReadLocker locker(this->mpLock);
  MatDataPtr cloned(new MatData(mData.clone()));
  locker.unlock();
  return cloned;
}

unsigned int cedar::aux::MatData::getDimensionality() const
{
  return cedar::aux::math::getDimensionalityOf(this->mData);
}
