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

    File:        pairsGame.h

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_PAIRSGAME_H
#define CEDAR_PROC_STEPS_PAIRSGAME_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include <cedar/auxiliaries/UIntVectorParameter.h>

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/pairsGame.fwd.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::proc::steps::pairsGame : public cedar::proc::Step
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  pairsGame();

public slots:
    void pairsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
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
  //!@brief Method that is called whenever an input is connected to the Connectable.
  virtual void inputConnectionChanged(const std::string& inputName);
    void paintPairs();
    void paintDebug1();
    void paintDebug2();
    void paintDebug3();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
  cedar::aux::MatDataPtr mOutputD1;
  cedar::aux::MatDataPtr mOutputD2;
  cedar::aux::MatDataPtr mOutputD3;

private:
    cedar::aux::UIntVectorParameterPtr _mSizes;
    cedar::aux::UIntParameterPtr _mTileSize;
    cedar::aux::UIntParameterPtr _mPadding;
    cedar::aux::UIntParameterPtr _mNumberOfPairs;
    cedar::aux::BoolParameterPtr _mSingleFeature;

    std::vector<cv::Rect> positions;
    std::vector<cv::Scalar> colors;
    std::vector<cv::Scalar> colorsSF;
    std::vector<float> orientations;
    std::vector<int> solved;
    int rot;
}; // class cedar::proc::steps::pairsGame

#endif // CEDAR_PROC_STEPS_PAIRSGAME_H
