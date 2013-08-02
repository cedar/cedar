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
#include <boost/lexical_cast.hpp>
#include<iostream>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::MatData::~MatData()
{
}

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
      }
      else
      {
        for (unsigned int i = 0; i < dim; ++i)
        {
          if (i > 0)
          {
            description += " x ";
          }
          description += cedar::aux::toString(mat.size[i]);
        }
      }
    }
    description += "<br />";
    description += "type: " + cedar::aux::math::matrixTypeToString(mat) + "<br />";
    description += "channels: " + cedar::aux::toString(mat.channels());
  }

  this->unlock();

  return description;
}

void cedar::aux::MatData::serializeData(std::ostream& stream)
{

  // todo input parameter stream
  this->lockForRead();

  //creating index that addresses an element in the n dimensional Mat
  std::vector<int> index;
  for( int k=0; k<mData.dims; k++ )
  {
    index.push_back(0);
  }

  //iterate  as long the last dimension has exceeded
  while(index[mData.dims-1] < mData.size[mData.dims-1])
  {
    // get memory address of the element
    uchar* element = mData.data;
    for(int i =0; i <mData.dims;i++)
    {
      element += mData.step[i]*index[i];
    }
    //check data type
    for(int i=0;i<mData.channels();i++)
    {
      //todo top
      switch(mData.depth())
      {
        case CV_8U:
        {
          stream << (int)(*(uchar*)(element+i*1)) << ",";
          break;
        }
        case CV_8S:
        {
          stream  << (int)(*(schar*)(element+i*1)) << ",";
          break;
        }
        case CV_16U:
        {
          stream  << *(unsigned short*)(element+i*2) << ",";
          break;
        }
        case CV_16S:
        {
          stream  << *(short*)(element+i*2) << ",";
          break;
        }
        case CV_32S:
        {
          stream  << *(int*)(element+i*4) << ",";
          break;
        }
        case CV_32F:
        {
          stream  << *(float*)(element+i*4) << ",";
          break;
        }
        case CV_64F:
        {
          stream  << *(double*)(element+i*8) << ",";
          break;
        }
      }
    }

    //increase index
    index[0]++;
    for(int i =0; i < mData.dims-1;i++)
    {
      if(index[i]>=mData.size[i])
      {
        index[i]=0;
        index[i+1]++;
      }
    }
  }

  this->unlock();
}

void cedar::aux::MatData::serializeHeader(std::ostream& stream)
{
  this->lockForRead();
  stream << "Mat" << ",";
  stream << mData.type() << ",";
  stream << mData.dims << ",";
  for(int i =0; i < mData.dims;i++)
  {
    stream << mData.size[i] << ",";
  }
  this->unlock();
}

cedar::aux::DataPtr cedar::aux::MatData::clone()
{
  lockForRead();
  MatDataPtr cloned(new MatData(mData.clone()));
  unlock();
  return cloned;
}

unsigned int cedar::aux::MatData::getDimensionality() const
{
  return cedar::aux::math::getDimensionalityOf(this->mData);
}
