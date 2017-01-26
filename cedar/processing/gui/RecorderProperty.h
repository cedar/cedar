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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 08 19

    Description: Header for the @em cedar::aux::gui::RecorderProperty class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_RECORDER_PROPERTY_H
#define CEDAR_PROC_GUI_RECORDER_PROPERTY_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"

// FORWARD DECLARATIONS
#include "cedar/processing/DataSlot.fwd.h"
#include "cedar/processing/gui/RecorderWidget.fwd.h"
#include "cedar/processing/gui/RecorderProperty.fwd.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <string>

/*!@brief With help of this class, the user is able to register, unregister and edit single DataSlots in the Recorder.
 */
class cedar::proc::gui::RecorderProperty : public QHBoxLayout
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The public constructor.
  RecorderProperty(cedar::proc::gui::RecorderWidget* parent, cedar::proc::DataSlotPtr slot);

  //!@brief The public destructor. 
  ~RecorderProperty();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  //!@brief Register this slot in the recorder. 
  void registerRecordData(bool status);

  //!@brief Informs the recorder that the step size for this slot has changed. 
  void updateStepSize(int value);

signals:
  //! signals change
  void changed();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief A check box for register and unregister the slot.
  QCheckBox* mCheckBox;

  //!@brief A spin box to set the record interval of the slot.
  QSpinBox* mStepSize; 

  //!@brief The pointer to the data of the slot.
  cedar::proc::DataSlotPtr mDataSlot;

  //!@brief Stores the record interval of the slot. Is changed by the mStepSize spin box.
  cedar::unit::Time mStepSizeValue;

  //!@brief A pointer that leads to the RecorderWidget;
  cedar::proc::gui::RecorderWidget* mRecorderWidget;
};

#endif // CEDAR_PROC_GUI_RECORDER_PROPERTY_H
