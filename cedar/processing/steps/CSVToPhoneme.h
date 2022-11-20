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

    File:        CSVToPhoneme.h

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2022 11 20

    Description: Header file for the class cedar::proc::steps::CSVToPhoneme.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CSVTO_PHONEME_H
#define CEDAR_PROC_STEPS_CSVTO_PHONEME_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/CSVToPhoneme.fwd.h"

// SYSTEM INCLUDES

class cedar::proc::steps::CSVToPhoneme : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CSVToPhoneme();

  //!@brief Destructor
  virtual ~CSVToPhoneme();

  //--------------------------------------------------------------------------------------------------------------------
  // public slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //@called when the vector dimension changes
  void vectorDimensionChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
  void compute(const cedar::proc::Arguments&);
private:
  // returns i-th slots name
  std::string makeSlotName(const int i);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::MatDataPtr _mInput;

  std::vector<cedar::aux::MatDataPtr> _mOutputs;

  float _mElapsedTime;
  bool _mIsLogging;
  bool _mHasLogged;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::FileParameterPtr _mCSVPath;
  cedar::aux::DoubleParameterPtr _mLogThreshold;
  //Parameter for the dimension of the output vector
  cedar::aux::UIntParameterPtr _mOutputDimension;

}; // class cedar::proc::steps::CSVToPhoneme

#endif // CEDAR_PROC_STEPS_CSVTO_PHONEME_H

