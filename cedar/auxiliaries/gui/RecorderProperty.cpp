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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 08 19

    Description: Implementation of the @em cedar::aux::gui::RecorderProperty class.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "RecorderProperty.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#include <QLabel>

cedar::aux::gui::RecorderProperty::RecorderProperty(const std::string& stepName, cedar::proc::DataSlotPtr slot)
{
  //Get slot porpertys.
  mStepName = stepName;
  mName = slot->getName();
  mData = slot->getData();
  bool registered = cedar::aux::RecorderSingleton::getInstance()->isRegistered(mData);
  std::cout << mName << " is registered: " << registered << std::endl;

  //Create name.
  QLabel* label = new QLabel(QString(mName.c_str()));
  //label->setMinimumWidth(180);
  this->addWidget(label);
  
  //Create spacer.
  QWidget* empty= new QWidget();
  empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);  
  this->addWidget(empty);

  //Create check box.
  mCheckBox = new QCheckBox();
  mCheckBox->setChecked(registered);
  connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(registerRecordData(int)));
  this->addWidget(mCheckBox);

  //Create spin bix.
  mStepSize = new QSpinBox();
  mStepSize->setMaximum(1000);
  mStepSize->setMinimum(0);  
  if(registered)
    mStepSize->setValue(cedar::aux::RecorderSingleton::getInstance()->getRecordIntervalTime(mStepName+"_"+mName));  
  else
    mStepSize->setValue(200); 
    mStepSizeValue= 200;
  mStepSize->setEnabled(registered);
  connect(mStepSize, SIGNAL(valueChanged(int)), this, SLOT(updateStepSize(int)));
  this->addWidget(mStepSize);
  QLabel* ms = new QLabel(QString("ms"));
  this->addWidget(ms);
  
}

cedar::aux::gui::RecorderProperty::~RecorderProperty()
{  
  QLayoutItem *item;
  while((item = this->takeAt(0))) 
  {
    delete item->widget();
  }
}

void cedar::aux::gui::RecorderProperty::registerRecordData(int status)
{  
  if(status)
  {
    if(!cedar::aux::RecorderSingleton::getInstance()->isRegistered(mStepName+"_"+mName))
    {
      cedar::aux::RecorderSingleton::getInstance()->registerData(mData, mStepSizeValue, mStepName+"_"+mName);
    }
    mStepSize->setEnabled(true);
  }
  else
  {
    cedar::aux::RecorderSingleton::getInstance()->unregisterData(mStepName+"_"+mName); 
    mStepSize->setEnabled(false);
  }   
}

void cedar::aux::gui::RecorderProperty::updateStepSize(int value)
{
  mStepSizeValue = (int)value;
  cedar::aux::RecorderSingleton::getInstance()->setRecordIntervalTime(mStepName+"_"+mName, (unsigned int)value);
}
