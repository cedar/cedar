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

void cedar::aux::MatData::serialize(std::ostream& stream) const
{
  this->serializeHeader(stream);
  stream << std::endl;
  this->serializeData(stream);
}

void cedar::aux::MatData::deserialize(std::istream& stream)
{
  // read header
  std::string header;
  std::getline(stream, header);

  std::string data;
  std::getline(stream, data);

  std::vector<std::string> header_entries;
  cedar::aux::split(header, ",", header_entries);

  CEDAR_ASSERT(header_entries.size() > 2);
  std::string data_type = header_entries.at(0);
  std::string mat_type_str = header_entries.at(1);

  CEDAR_ASSERT(data_type == "Mat");

  int mat_type = cedar::aux::math::matrixTypeFromString(mat_type_str);
  std::vector<int> sizes;

  // read the matrix size
  for (size_t i = 2; i < header_entries.size(); ++i)
  {
    sizes.push_back(cedar::aux::fromString<int>(header_entries.at(i)));
  }

  cv::Mat mat(static_cast<int>(sizes.size()), &sizes.front(), mat_type);

  // read data
  std::vector<std::string> data_entries;
  cedar::aux::split(data, ",", data_entries);

  std::vector<int> index(sizes.size(), 0);
  // currently, not implemented for multiple channels
  CEDAR_ASSERT(mat.channels() == 1);
  for (const auto& data_str : data_entries)
  {
    switch (mat.type())
    {
      case CV_32F:
        mat.at<float>(&index.front()) = cedar::aux::fromString<float>(data_str);
        break;

      default:
        // not implemented for this type
        CEDAR_ASSERT(false);
    }
    ++index[0];
    for (int i = 0; i < mat.dims - 1; ++i)
    {
      if (index[i] >= mat.size[i])
      {
        index[i] = 0;
        ++index[i+1];
      }
    }
  }

  QWriteLocker locker(this->mpLock);
  this->setData(mat);
}

void cedar::aux::MatData::serializeData(std::ostream& stream) const
{
  QReadLocker locker(this->mpLock);
  //creating index that addresses an element in the n dimensional Mat
  std::vector<int> index(mData.dims, 0);

  bool first = true;
  //iterate  as long the last dimension has exceeded
  while (index[mData.dims-1] < mData.size[mData.dims-1])
  {
    if (first)
    {
      first = false;
    }
    else
    {
      stream << ",";
    }

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
          stream << static_cast<int>(element[i]);
          break;
        }
        case CV_8S:
        {
          stream << static_cast<int>(reinterpret_cast<schar*>(element)[i]);
          break;
        }
        case CV_16U:
        {
          stream << reinterpret_cast<unsigned short*>(element)[i];
          break;
        }
        case CV_16S:
        {
          stream << reinterpret_cast<short*>(element)[i];
          break;
        }
        case CV_32S:
        {
          stream << reinterpret_cast<int*>(element)[i];
          break;
        }
        case CV_32F:
        {
          stream << reinterpret_cast<float*>(element)[i];
          break;
        }
        case CV_64F:
        {
          stream << reinterpret_cast<double*>(element)[i];
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
    if (i > 0)
    {
      stream << ",";
    }
    stream << mData.size[i];
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
