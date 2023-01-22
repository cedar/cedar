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
    Date:        2022 12 08

    Description: Header file for the class cedar::dyn::steps::CSVToPhoneme.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_STEPS_CSVTO_PHONEME_H
#define CEDAR_DYN_STEPS_CSVTO_PHONEME_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/StringParameter.h"


// FORWARD DECLARATIONS
#include "cedar/dynamics/steps/CSVToPhoneme.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */

//todo add briefs to all methods
class cedar::dyn::steps::CSVToPhoneme : public cedar::dyn::Dynamics
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------q
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
  void csvPathChanged();
  void delimiterChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Updates the output matrix.
  void eulerStep(const cedar::unit::Time& time);

  //!@brief returns i-th slots name
  std::string makeSlotName(const int i);

  void reloadLookupTable();

  void setAllOutputsToValue(int value);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mInput;

  std::string inputName = "input";

  std::vector<cedar::aux::MatDataPtr> mOutputs;

  float mElapsedTime;
  bool mDoneWithCVS;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::FileParameterPtr mCSVPath;
  cedar::aux::DoubleParameterPtr mStartingThreshold;
  //Parameter for the dimension of the output vector
  cedar::aux::UIntParameterPtr mOutputDimension;
  std::vector<int> mLookupTable;
  cedar::aux::StringParameterPtr mDelimiter;
}; // class cedar::dyn::steps::CSVToPhoneme

#endif // CEDAR_DYN_STEPS_CSVTO_PHONEME_H

