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

    File:        CSVToPhoneme.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2022 12 08

    Description: Source file for the class cedar::dyn::steps::CSVToPhoneme.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/dynamics/steps/CSVToPhoneme.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include <boost/algorithm/string.hpp>

// CEDAR INCLUDES

// SYSTEM INCLUDES

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
            (
                    new ElementDeclarationTemplate<cedar::dyn::steps::CSVToPhoneme>
                            (
                                    "Sources",
                                    "cedar.dynamics.CSVToPhoneme"
                            )
            );
    //declaration->setIconPath(":/steps/log_time_interval.svg");
    declaration->setDescription
            (
                    //todo add description
                    "CSV To Phoneme Step"
            );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dyn::steps::CSVToPhoneme::CSVToPhoneme():
        mInput(cedar::aux::MatDataPtr()),
        mOutputs(1, cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))),
        mElapsedTime(0.0),
        mCSVPath(new cedar::aux::FileParameter(this, "CSV file", cedar::aux::FileParameter::Mode::READ,
                                               "example.txt")),
        mLogThreshold(new cedar::aux::DoubleParameter(this, "log threshold", 0.5)),
        mOutputDimension(new cedar::aux::UIntParameter(this, "dimensionality", 1, 1, 255)),
        mDoneWithCVS(false)
{
  this->declareInput("input", false);

  this->declareOutput(makeSlotName(0), mOutputs.front());


  QObject::connect(mOutputDimension.get(), SIGNAL(valueChanged()), this, SLOT(vectorDimensionChanged()));
  QObject::connect(mCSVPath.get(), SIGNAL(valueChanged()), this, SLOT(csvPathChanged()));
}

cedar::dyn::steps::CSVToPhoneme::~CSVToPhoneme()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::steps::CSVToPhoneme::eulerStep(const cedar::unit::Time& time)
{
  mElapsedTime +=  time / cedar::unit::Time(1*cedar::unit::milli * cedar::unit::seconds);

  std::cout << "Seconds passed: " << mElapsedTime/1000 << std::endl;

  if(!mDoneWithCVS)
  {
    if(mElapsedTime <= mLookupTable.size())
    {
      //todo add comments here
      if (mLookupTable.at(mElapsedTime) == 0)
      {
        setAllOutputsToValue(0);
      }
      else
      {
        //-1 because LookUp Table starts with 1, mOutputs vector internally starts with 0 (but also starts with 1 in naming in the tooltip/id)
        int ActivatedPhonemeIndex = mLookupTable.at(mElapsedTime) - 1;
        mOutputs.at(ActivatedPhonemeIndex)->getData().setTo(1);

        //Edge case: if there is not time between 2 phonemes, the first will be kept on till the first 0 comes again. Therefore set all outputs to 0 except current
        for (auto output: mOutputs)
        {
          if (output == mOutputs.at(ActivatedPhonemeIndex))
            continue;

          output->getData().setTo(0);
        }
      }
    }
    else
    {
      mDoneWithCVS = true;
      setAllOutputsToValue(0);
    }
  }
}


void cedar::dyn::steps::CSVToPhoneme::csvPathChanged()
{
  reloadLookupTable();

  this->reset();
}

void cedar::dyn::steps::CSVToPhoneme::reloadLookupTable()
{
  //todo: check if file exists, is readable etc.
  std::string delimeter = ",";
  std::ifstream CSVFile(mCSVPath->getPath());

  //each vector in data is a line of the csv (also represented as vector)
  //in line 0=unique phoneme index, 1=phoneme string, 2=starting time in seconds, 3=ending times in second
  //todo: remove this magic number stuff. Use a better datastructure where the index gets replaced by string (e.g data[0]["ending_time"])
  //todo: other name than 'data'
  std::vector<std::vector<std::string>> data;
  if(CSVFile.good())
  {
    std::string line = "";
    // Iterate through each line and split the content using delimeter
    while (getline(CSVFile, line))
    {
      //Skip empty lines
      if(!(line.empty() || line.find_first_not_of(' ') == std::string::npos))
      {
        std::vector<std::string> vec;
        boost::split(vec, line, boost::is_any_of(delimeter));
        data.push_back(vec);
      }

    }
    // Close the File
    CSVFile.close();
  }
  else
  {
    //todo: throw some correct extection. which ertype here?
  }

  //Go to last line and find size and init lookup table with 0 values
  // *1000 so seconds get converted into MS
  int endingTimeInMSOfLastEntry = std::stof(data.back().at(3)) * 1000;

  //fill lookuptable with 0
  for (int i=0; i <= endingTimeInMSOfLastEntry; i++)
  {
    mLookupTable.push_back(0);
  }

  //now iterate through all data entries and fill indexes from starting time to ending time with correpsoinding unqiue value of lookup table
  for(auto singlePhonemeEntry : data)
  {
    int unqiuePhonemeIdentifierIndex = std::stoi(singlePhonemeEntry.at(0));
    int startingTimeInMS = std::stof(singlePhonemeEntry.at(2)) * 1000;
    int endingTimeInMS = std::stof(singlePhonemeEntry.at(3)) * 1000;

    for(int i=startingTimeInMS; i <= endingTimeInMS; i++)
    {
      mLookupTable.at(i) = unqiuePhonemeIdentifierIndex;
    }
  }

  //Check the highest value of the lookup table. This will be the number of output dimensions
  //todo: no idea what the *does check before pushing
  int maxValueOfPhonemeIndenitifier = *std::max_element(mLookupTable.begin(), mLookupTable.end());

  //set output dim to max
  mOutputDimension->setValue(maxValueOfPhonemeIndenitifier);

  //print lookup table - for debugging
  /*for(int i = 0; i < mLookupTable.size(); i++)
  {
    std::cout << "index: " << i << " Output" << mLookupTable.at(i) << std::endl;
  }*/
}

void cedar::dyn::steps::CSVToPhoneme::vectorDimensionChanged()
{
  unsigned newsize = mOutputDimension->getValue();

  if (newsize == mOutputs.size())
  {
    return;
  }
  else if (newsize < mOutputs.size())
  {
    //delete unused slots
    for (unsigned i=newsize; i < mOutputs.size(); i++)
    {
      removeOutputSlot(makeSlotName(i));
    }
  }
  else if (newsize > mOutputs.size())
  {
    //declare new output slots
    for (unsigned i = mOutputs.size(); i < newsize; i++)
    {
      //declare new output and append it to list
      cedar::aux::MatDataPtr output = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)));
      mOutputs.push_back(output);

      declareOutput(makeSlotName(i), mOutputs.at(i));
    }
  }
  //resize inputs vector
  mOutputs.resize(newsize);

  //todo is this needed? Makes an error since it calls the compute method once and gives an error
  //onTrigger();
}

void cedar::dyn::steps::CSVToPhoneme::reset()
{
  mElapsedTime = 0;
  mDoneWithCVS = false;
}

void cedar::dyn::steps::CSVToPhoneme::setAllOutputsToValue(int value)
{
  for (auto output: mOutputs)
  {
    output->getData().setTo(value);
  }
}

std::string cedar::dyn::steps::CSVToPhoneme::makeSlotName(const int i)
{
  std::stringstream s;
  s << "phoneme " << i+1;
  return s.str();
}