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

    File:        Matrix.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 11

    Description: Source file for the class cedar::proc::typecheck::Matrix.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::typecheck::Matrix::Matrix()
:
mAcceptsEmptyMatrix(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::typecheck::Matrix::addAcceptedDimensionality(unsigned int dimensionality)
{
  this->mAcceptedDimensionalities.push_back(dimensionality);
}

void cedar::proc::typecheck::Matrix::addAcceptedDimensionalityRange(unsigned int lowest, unsigned int highest)
{
  //!@todo This is not the best way of storing this information. Rather, a class is needed that stores a set of mins and maxes
  for (unsigned int d = lowest; d <= highest; ++d)
  {
    this->addAcceptedDimensionality(d);
  }
}

void cedar::proc::typecheck::Matrix::addAcceptedNumberOfChannels(unsigned int numberOfChannels)
{
  this->mAcceptedNumberOfChannels.push_back(numberOfChannels);
}

void cedar::proc::typecheck::Matrix::addAcceptedType(int type)
{
  this->mAcceptedTypes.push_back(type);
}

void cedar::proc::typecheck::Matrix::acceptsEmptyMatrix(bool accepts)
{
  this->mAcceptsEmptyMatrix = accepts;
}

void cedar::proc::typecheck::Matrix::addAcceptedMinimumSizes(std::vector<boost::optional<unsigned int> > minimumSizes)
{
  this->mAcceptedMinimumSizes[minimumSizes.size()] = minimumSizes;
}

cedar::proc::DataSlot::VALIDITY
  cedar::proc::typecheck::Matrix::check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data, std::string& info) const
{
  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->isEmpty())
    {
      if (this->mAcceptsEmptyMatrix)
      {
        return this->validityOk();
      }
      else
      {
        info = "Got an empty matrix, but cannot handle empty matrices.";
        return this->validityBad();
      }
    }

    bool type_ok = this->mAcceptedTypes.empty(); // if empty, everything is accepted
    for (size_t i = 0; i < this->mAcceptedTypes.size(); ++i)
    {
      if (mat_data->getCvType() == this->mAcceptedTypes.at(i))
      {
        type_ok = true;
        break;
      }
    }
    if (!type_ok)
    {
      info = "Cannot handle the matrix type \"" + cedar::aux::math::matrixTypeToString(mat_data->getData()) + "\".";
      return this->validityBad();
    }

    bool dim_ok = this->mAcceptedDimensionalities.empty(); // if empty, everything is accepted
    for (size_t i = 0; i < this->mAcceptedDimensionalities.size(); ++i)
    {
      if (mat_data->getDimensionality() == this->mAcceptedDimensionalities.at(i))
      {
        dim_ok = true;
        break;
      }
    }
    if (!dim_ok)
    {
      info = "Cannot handle this dimensionality.";
      return this->validityBad();
    }

    // check if any limits for minimum sizes are set
    auto accepted_sizes = this->mAcceptedMinimumSizes.find(mat_data->getDimensionality());
    if (accepted_sizes != this->mAcceptedMinimumSizes.end())
    {
      for (unsigned int i = 0; i < mat_data->getDimensionality(); ++i)
      {
        if (accepted_sizes->second.at(i).is_initialized())
        {
          if (accepted_sizes->second.at(i).get() > static_cast<unsigned int>(mat_data->getData().size[i]))
          {
            info = "The size of dimension " + cedar::aux::toString(i) + " is smaller than the required minimum size of "
                   + cedar::aux::toString(accepted_sizes->second.at(i).get()) + ".";
            return this->validityBad();
          }
        }
      }
    }

    bool channel_ok = this->mAcceptedNumberOfChannels.empty(); // if empty, everything is accepted
    for (size_t i = 0; i < this->mAcceptedNumberOfChannels.size(); ++i)
    {
      if (static_cast<unsigned int>(mat_data->getData().channels()) == this->mAcceptedNumberOfChannels.at(i))
      {
        channel_ok = true;
        break;
      }
    }
    if (!channel_ok)
    {
      info = "Cannot handle this amount of channels.";
      return this->validityBad();
    }

    return this->validityOk();
  }

  info = "Expected MatData, but got something else.";
  return this->validityBad();
}
