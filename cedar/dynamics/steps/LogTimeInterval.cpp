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

    File:        LogTimeInterval.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 04 18

    Description: Source file for the class cedar::dyn::steps::LogTimeInterval.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/dynamics/steps/LogTimeInterval.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

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
                    new ElementDeclarationTemplate<cedar::dyn::steps::LogTimeInterval>
                            (
                                    "Programming",
                                    "cedar.dynamics.LogTimeInterval"
                            )
            );
    declaration->setIconPath(":/steps/log_time_interval.svg");
    declaration->setDescription
            (
                    "Logs the time between two peak detector signals"
            );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dyn::steps::LogTimeInterval::LogTimeInterval():
        _mElapsedTime(0.0),
        _mIsLogging(false),
        _mHasLogged(false),
        _mLogPath(new cedar::aux::FileParameter(this,"log file",cedar::aux::FileParameter::Mode::WRITE,"example.txt")),
        _mLogPrefix(new cedar::aux::StringParameter(this,"log prefix","")),
        _mLogThreshold(new cedar::aux::DoubleParameter(this, "log threshold", 0.5))
{
   this->declareInput(inputFirstName);
   this->declareInput(inputSecondName);
}

cedar::dyn::steps::LogTimeInterval::~LogTimeInterval()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::steps::LogTimeInterval::eulerStep(const cedar::unit::Time& time)
{

  if (_mInputFirst && _mInputSecond)
  {
    if(_mInputFirst->getData().at<float>(0,0) > this->_mLogThreshold->getValue() && !_mIsLogging && !_mHasLogged)
    {
      _mIsLogging = true;
      _mElapsedTime = 0;
      std::cout << "Logging begins" << std::endl;
    }

    if (_mInputSecond->getData().at<float>(0, 0) > this->_mLogThreshold->getValue() && _mIsLogging)
    {
      _mIsLogging = false;
      _mHasLogged = true;
      std::cout << "Logging Ends! It took "<< _mElapsedTime<<" seconds!" << std::endl;
      this->print(_mElapsedTime);
    }
    _mElapsedTime +=  time / cedar::unit::Time(1*cedar::unit::milli * cedar::unit::seconds);
  }
}


void cedar::dyn::steps::LogTimeInterval::inputConnectionChanged(const std::string& inputName)
{
  // Assign the input to the member. This saves us from casting in every computation step.
  if(inputName == inputFirstName)
  {
    this->_mInputFirst = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  if(inputName == inputSecondName)
  {
    this->_mInputSecond = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::steps::LogTimeInterval::determineInputValidity(cedar::proc::ConstDataSlotPtr /*slot*/, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input && _input->getDimensionality() == 0)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::steps::LogTimeInterval::reset()
{
  _mHasLogged = false;
  _mIsLogging = false;
}


void cedar::dyn::steps::LogTimeInterval::print(float loggedTime)
{
  std::string filepath = _mLogPath->getPath();

  if(!_mFile.is_open()&&filepath.find(".txt")!=std::string::npos)
  {
    _mFile.open(filepath,std::ofstream::app);
  }

  _mFile <<_mLogPrefix->getValue()<<"\t"<<boost::lexical_cast<std::string>(loggedTime)<<std::endl;

  _mFile.close();

}