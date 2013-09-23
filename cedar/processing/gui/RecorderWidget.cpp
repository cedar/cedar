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

    Description: Implementation of the @em cedar::aux::gui::RecorderWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/RecorderWidget.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/gui/RecorderProperty.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>


cedar::proc::gui::RecorderWidget::RecorderWidget()
{
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);
}

cedar::proc::gui::RecorderWidget::RecorderWidget(QWidget* pParent)
:
QWidget(pParent)
{
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);
}

cedar::proc::gui::RecorderWidget::~RecorderWidget()
{
  delete mMainLayout;
}


void cedar::proc::gui::RecorderWidget::setStep(cedar::proc::StepPtr step)
{
  this->mStepToConfigure = step;
  connect(step.get(), SIGNAL(nameChanged()), this, SLOT(updateName()));
  refreshWidget();
}
void cedar::proc::gui::RecorderWidget::refreshWidget()
{
  // reset the widget
  clearLayout();

  // Check if step not null
  if (!mStepToConfigure)
  {
    return;
  }

  // draw Headers
  this->createHeader(mStepToConfigure->getName());

  // create recorder properties

  // first get buffers
  //!@todo Redundant code below - again make a list of roles over which you then iterate
  if(mStepToConfigure->hasRole(cedar::proc::DataRole::BUFFER))
  {
    const cedar::proc::Connectable::SlotList buffer_slots = mStepToConfigure->getOrderedDataSlots(cedar::proc::DataRole::BUFFER);
    if (buffer_slots.size() > 0)
    {
      createRoleSection("Buffers"); //!@todo Don't hard-code the "Buffers" string - we have a function for that somewhere
    }
    
    for (unsigned int i = 0; i < buffer_slots.size(); i++)
    {
      mMainLayout->addLayout(new RecorderProperty(mStepToConfigure->getName(), buffer_slots[i]));
    }
  }

  //get outputs
  if(mStepToConfigure->hasRole(cedar::proc::DataRole::OUTPUT))
  {
    const cedar::proc::Connectable::SlotList  output_slots = mStepToConfigure->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT);
    if (output_slots.size()>0)
    {
      createRoleSection("Outputs"); //!@todo See comment above
    }
    for(unsigned int i = 0; i < output_slots.size(); i++)
    {
      mMainLayout->addLayout(new RecorderProperty(mStepToConfigure->getName(), output_slots[i]));
    }
  }
  mMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void cedar::proc::gui::RecorderWidget::clearLayout()
{ 
  QLayoutItem *item;
  while ((item = mMainLayout->takeAt(0)) != NULL)
  {
    QLayoutItem* widget;
    while((widget = item->layout()->takeAt(0)) != NULL)
    {
      delete widget->widget();
    }
    delete item->layout();
  }
}

void cedar::proc::gui::RecorderWidget::resetContents()
{
  //reset the widget
  //!@todo What's the purpose of resetContents, if all it does is call another function?
  this->clearLayout();
}

void cedar::proc::gui::RecorderWidget::createHeader(const std::string& name)
{  
  QHBoxLayout* step_name_layout = new QHBoxLayout();
  //Create step name.
  QLabel* step_name = new QLabel(QString(name.c_str()));
  //!@todo Don't hard-code fonts, use the existing font of the label and modify it.
  step_name->setFont(QFont("Arial", 12, QFont::Bold));
  step_name_layout->addWidget(step_name);

  mMainLayout->addLayout(step_name_layout);
  

  QHBoxLayout* row_headers = new QHBoxLayout();
  // Create header for the slot names.
  QLabel* label = new QLabel("Slot name");
  row_headers->addWidget(label);
  
  // Create spacer.
  QWidget* empty = new QWidget();
  empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);  
  row_headers->addWidget(empty);

  // Create row header for record time.
  QLabel* label2 = new QLabel("Record interval");
  row_headers->addWidget(label2);
  mMainLayout->addLayout(row_headers);

  // Create spacer row.
  QHBoxLayout* spacer_layout = new QHBoxLayout();
  QWidget* empty2 = new QWidget();
  empty2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);  
  spacer_layout->addWidget(empty2);
  mMainLayout->addLayout(spacer_layout);
}

void cedar::proc::gui::RecorderWidget::createRoleSection(const std::string& name)
{     
  QHBoxLayout* role_font_layout = new QHBoxLayout();

  // Create role name.
  QLabel* role_name = new QLabel(QString::fromStdString(name));
  QFont role_font = role_name->font();
  role_font.setPointSize(role_font.pointSize() + 2);
  role_font.setBold(true);
  role_name->setFont(role_font);
  role_font_layout->addWidget(role_name);

  mMainLayout->addLayout(role_font_layout);
}


void cedar::proc::gui::RecorderWidget::unregister(cedar::proc::StepPtr pStep)
{
  //!@todo This code is also redundant (the two ifs are almost exactly the same); to avoid: make a list of data roles, iterate over it
  //!@todo This code is in the wrong place: the gui is not responsible for unregistering steps in the recorder, the steps should do that themselves.
  // unregister buffers
  if (pStep->hasRole(cedar::proc::DataRole::BUFFER))
  {
    const cedar::proc::Connectable::SlotList buffer_slots = pStep->getOrderedDataSlots(cedar::proc::DataRole::BUFFER);
    for (unsigned int i = 0; i < buffer_slots.size(); ++i)
    {
      //!@todo The way of encoding the step/buffer name is repeated a lot. Move this into a function (e.g., in case we ever want to replace, the '_' by something else, one would have to change this in a large number of places)
      cedar::aux::RecorderSingleton::getInstance()->unregisterData(pStep->getName() + "_" + buffer_slots[i]->getName());
    }
  }

  // unregister outputs
  if (pStep->hasRole(cedar::proc::DataRole::OUTPUT))
  {
    const cedar::proc::Connectable::SlotList output_slots = pStep->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT);
    for (unsigned int i = 0; i < output_slots.size(); ++i)
    {
      cedar::aux::RecorderSingleton::getInstance()->unregisterData(pStep->getName() + "_" + output_slots[i]->getName());
    }
  }
}

void cedar::proc::gui::RecorderWidget::updateName()
{
  if (cedar::proc::Step* pStep = dynamic_cast<cedar::proc::Step*>(QObject::sender()))
  {
    //!@todo Redundant again - see comment above (make a list of data roles).
    //unregister buffers
    if (pStep->hasRole(cedar::proc::DataRole::BUFFER))
    {
      const cedar::proc::Connectable::SlotList buffer_slots = pStep->getOrderedDataSlots(cedar::proc::DataRole::BUFFER);
      for (unsigned int i = 0; i < buffer_slots.size(); ++i)
      {
        if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(buffer_slots[i]->getData()))
        {
          cedar::aux::RecorderSingleton::getInstance()->renameRegisteredData
                                                        (
                                                          buffer_slots[i]->getData(),
                                                          pStep->getName() + "_" + buffer_slots[i]->getName()
                                                        );
        }
      }
    }

    //unregister outputs
    if (pStep->hasRole(cedar::proc::DataRole::OUTPUT))
    {
      const cedar::proc::Connectable::SlotList output_slots = pStep->getOrderedDataSlots(cedar::proc::DataRole::OUTPUT);
      for (unsigned int i = 0; i < output_slots.size(); ++i)
      {
        if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(output_slots[i]->getData()))
        {
          cedar::aux::RecorderSingleton::getInstance()->renameRegisteredData
                                                        (
                                                          output_slots[i]->getData(),
                                                          pStep->getName() + "_" + output_slots[i]->getName()
                                                        );
        }
      }
    }    
    this->refreshWidget();
  }
}
