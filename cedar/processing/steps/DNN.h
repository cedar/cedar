/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        DNN.h

    Maintainer:  Raul Grieben
    Email:       Raul.Grieben@ini.ruhr-uni-bochum.de
    Date:        2021 11 30

    Description: Header file for the class cedar::proc::steps::DNN.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_DNN_H
#define CEDAR_PROC_STEPS_DNN_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/DNN.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

/*!
 */
class cedar::proc::steps::DNN : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

    class dnnType
    {
    public:
        //! the id of an enum entry
        typedef cedar::aux::EnumId Id;

        //! constructs the enum for all ids
        static void construct()
        {
            mType.type()->def(cedar::aux::Enum(TL, "TL"));
            mType.type()->def(cedar::aux::Enum(B5, "B5"));
            mType.type()->def(cedar::aux::Enum(B4, "B4"));
            mType.type()->def(cedar::aux::Enum(B3, "B3"));
            mType.type()->def(cedar::aux::Enum(B2, "B2"));
            mType.type()->def(cedar::aux::Enum(B1, "B1"));
        }

        //! @returns A const reference to the base enum object.
        static const cedar::aux::EnumBase& type()
        {
            return *(mType.type());
        }

        //! @returns A pointer to the base enum object.
        static const cedar::proc::DataRole::TypePtr& typePtr()
        {
            return mType.type();
        }

    public:
        static const Id TL = 0;
        static const Id B5 = 1;
        static const Id B4 = 2;
        static const Id B3 = 3;
        static const Id B2 = 4;
        static const Id B1 = 5;
        
    private:
        static cedar::aux::EnumType<dnnType> mType;
    };

  //!@brief The standard constructor.
  DNN();

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
private:
  void compute(const cedar::proc::Arguments& arguments);

private slots:

    void updateDnnType();

    void recompute();
 

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::proc::InputSlotHelper<cedar::aux::MatData> mImage;

  //!@brief The output
  cedar::aux::MatDataPtr mOutput;

  cv::dnn::Net model;
  cv::Mat image_back;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
    cedar::aux::EnumParameterPtr _mDnnType;

}; // class cedar::proc::steps::DNN

#endif // CEDAR_PROC_STEPS_DNN_H

