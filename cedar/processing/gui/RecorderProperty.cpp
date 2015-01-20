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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 08 19

    Description: Implementation of the @em cedar::aux::gui::RecorderProperty class.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/processing/gui/RecorderProperty.h"
#include "cedar/processing/gui/RecorderWidget.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QLabel>

cedar::proc::gui::RecorderProperty::RecorderProperty(cedar::proc::gui::RecorderWidget* parent, const std::string& stepName, cedar::proc::DataSlotPtr slot)
{
  this->mRecorderWidget = parent;
  //Get slot properties.
  mStepName = stepName;
  mName = slot->getName();
  mData = slot->getData();
  bool registered = cedar::aux::RecorderSingleton::getInstance()->isRegistered(mData);

  //Create name.
  QLabel* label = new QLabel(QString(mName.c_str()));
  //label->setMinimumWidth(180);
  this->addWidget(label);
  
  //Create spacer.
  QWidget* empty = new QWidget();
  empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);  
  this->addWidget(empty);

  //Create check box.
  mCheckBox = new QCheckBox();
  mCheckBox->setChecked(registered);
  connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(registerRecordData(int)));
  connect(mCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));
  this->addWidget(mCheckBox);

  //Create spin box.
  mStepSize = new QSpinBox();
  mStepSize->setMaximum(1000);
  mStepSize->setMinimum(0);

  {
    using namespace cedar::unit;
    if (registered)
    {
      Time milli_second(1.0 * milli * second);
      mStepSize->setValue(cedar::aux::RecorderSingleton::getInstance()->getRecordIntervalTime(mStepName + "_" + mName) / milli_second);
    }
    else
    {
      // step size in ms
      double value = 200.0;

      mStepSize->setValue(static_cast<int>(value));
      mStepSizeValue = Time(value * milli * seconds);
    }
  }
  mStepSize->setEnabled(registered);
  connect(mStepSize, SIGNAL(valueChanged(int)), this, SLOT(updateStepSize(int)));
  connect(mStepSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
  mStepSize->setSuffix(" ms");
  this->addWidget(mStepSize);
  
}

cedar::proc::gui::RecorderProperty::~RecorderProperty()
{  
  QLayoutItem *item;
  while ((item = this->takeAt(0)) != NULL)
  {
    delete item->widget();
  }
}

void cedar::proc::gui::RecorderProperty::registerRecordData(int status)
{  
  if (status)
  {
    if (!cedar::aux::RecorderSingleton::getInstance()->isRegistered(mStepName + "_" + mName))
    {
      cedar::aux::RecorderSingleton::getInstance()->registerData(mData, mStepSizeValue, mStepName + "_" + mName);
    }
    mStepSize->setEnabled(true);
  }
  else
  {
    cedar::aux::RecorderSingleton::getInstance()->unregisterData(mStepName + "_" + mName); 
    mStepSize->setEnabled(false);
  }
  mRecorderWidget->emitStepRegisteredinRecorder();
}

void cedar::proc::gui::RecorderProperty::updateStepSize(int value)
{
  using namespace cedar::unit;
  Time step_size(static_cast<double>(value) * milli * seconds);

  mStepSizeValue = step_size;
  cedar::aux::RecorderSingleton::getInstance()->setRecordIntervalTime(mStepName + "_" + mName, step_size);
}
