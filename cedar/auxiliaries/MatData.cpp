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
  std::string detailed_description;

  this->lockForRead();
  const cv::Mat& mat = this->getData();
  if (mat.empty())
  {
    description += "Empty data.";
  }
  else
  {
    unsigned int dim = cedar::aux::math::getDimensionalityOf(mat);
    //description += cedar::aux::toString(dim) + "-dimensional matrix";

    // use the same color for sizes and dimensionalities as everywhere else:
    description += "<font color=\"#000080\">Tensor of order</font>: " + cedar::aux::toString(dim);
    if (dim != 0)
    {
      description += "<br /><font color=\"#000080\">Size</font>: ";
      if (dim == 1)
      {
        description += cedar::aux::toString(cedar::aux::math::get1DMatrixSize(mat));
        description += " (as ";
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
        description += " matrix)";
      }
    }
    else
    {
      description += " (as 1 x 1 matrix)";
    }

    //description += "<br />";
    detailed_description += "<table>";
    detailed_description += "<tr><td>Numeric type: </td><td>" + cedar::aux::math::matrixTypeToString(mat) + "</td></tr>";
    detailed_description += "<tr><td>Channels: </td><td>" + cedar::aux::toString(mat.channels()) + "</td>";

    unsigned int memsize;
    std::string memsize_extra= "";
    if (mat.isContinuous())
    {
      memsize= mat.total() * mat.elemSize();
      memsize_extra= " (continuous)";
    }
    else
    {
      for( int i= 0; i < mat.rows; i++ )
      {
        memsize += mat.step[i] * mat.rows;
      }
      memsize_extra= " (non continuous)";
    }

    detailed_description += "<tr><td>Bytes: </td><td>" 
                            + boost::lexical_cast<std::string>( memsize )
                            + memsize_extra
                            + "</td>";

    detailed_description += "</table><br />";                           
  }

  this->unlock();

  description += "<hr />";
  description += detailed_description;
  description += Super::getDescription();

  return description;
}

void cedar::aux::MatData::serialize(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const
{
  this->serializeHeader(stream, mode);
  stream << std::endl;
  this->serializeData(stream, mode);
}

void cedar::aux::MatData::deserialize(std::istream& stream, cedar::aux::SerializationFormat::Id mode)
{
  //!@todo The serialization mode should be deduced from the header, no need to pass it here.
  // read header
  std::string header;
  std::getline(stream, header);

  std::vector<std::string> header_entries;
  cedar::aux::split(header, ",", header_entries);

  CEDAR_ASSERT(header_entries.size() > 2);
  std::string data_type = header_entries.at(0);
  std::string mat_type_str = header_entries.at(1);

  CEDAR_ASSERT(data_type == "Mat");

  int mat_type = cedar::aux::math::matrixTypeFromString(mat_type_str);
  std::vector<int> sizes;

  // read the matrix size
  size_t offset = 0;
  if (header_entries.back() == "compact")
  {
    offset = 1;
    CEDAR_NON_CRITICAL_ASSERT(mode == cedar::aux::SerializationFormat::Compact);
  }
  for (size_t i = 2; i < header_entries.size() - offset; ++i)
  {
    sizes.push_back(cedar::aux::fromString<int>(header_entries.at(i)));
  }

  cv::Mat mat(static_cast<int>(sizes.size()), &sizes.front(), mat_type);

  // read data
  switch (mode)
  {
    case cedar::aux::SerializationFormat::CSV:
    {
      std::string data;
      std::getline(stream, data);

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
      break;
    } // case SERIALIZE_CSV

    case cedar::aux::SerializationFormat::Compact:
    {
      // matrix should be a linear array in memory, without gaps
      CEDAR_DEBUG_ASSERT(mat.isContinuous());

      // make sure the fail bit wasn't false before the operations
      CEDAR_ASSERT(!stream.fail());

      // initialize string with the appropriate size
      stream.read(reinterpret_cast<char*>(mat.data), static_cast<size_t>(mat.total() * mat.elemSize()));

      // make sure the fail bit wasn't set by the preceding operations
      CEDAR_ASSERT(!stream.fail());

      break;
    } // case SERIALIZE_COMPACT
  }

  QWriteLocker locker(this->mpLock);
  this->setData(mat);
}

void cedar::aux::MatData::serializeData(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const
{
  QReadLocker locker(this->mpLock);
  //creating index that addresses an element in the n dimensional Mat

  switch (mode)
  {
    case cedar::aux::SerializationFormat::CSV:
    {
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
          if (i != mData.channels() - 1)
          {
            stream << ",";
          }
        }

        //increase index
        //!@todo This is slow in multiple ways; a faster approach would be to iterate over the linear memory with a linear index
        //!@todo Also, we have an iterator class for iterating over a 3d matrix
        index[0]++;
        for (int i = 0; i < mData.dims-1; i++)
        {
          if (index[i] >= mData.size[i])
          {
            index[i] = 0;
            index[i+1]++;
          }
        }
      }
      break;
    } // case SerializationMode::SERIALIZE_CSV

    case cedar::aux::SerializationFormat::Compact:
    {
      // we can only handle matrices that are continuous, i.e., those, that are written linearly in memory
      CEDAR_ASSERT(this->mData.isContinuous());
      // create a string buffer that will hold the binary representation of all elements
      std::string buffer(this->mData.elemSize() * this->mData.total(), '\0');
      for (size_t i = 0; i < mData.total() * this->mData.elemSize(); ++i)
      {
        buffer[i] = this->mData.data[i];
      }
      stream.write(buffer.c_str(), buffer.size());
      break;
    }
  }

}

void cedar::aux::MatData::serializeHeader(std::ostream& stream, cedar::aux::SerializationFormat::Id mode) const
{
  //!@todo When any non-default serialization mode is used, this should probably be indicated in the header. Replace Mat by, e.g., CompactMat?
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

  if (mode == cedar::aux::SerializationFormat::Compact)
  {
    stream << ",compact";
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
