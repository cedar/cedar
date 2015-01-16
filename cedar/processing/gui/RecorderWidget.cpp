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

    Description: Implementation of the @em cedar::aux::gui::RecorderWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/RecorderWidget.h"
#include "cedar/processing/gui/RecorderProperty.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <vector>


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

  std::vector<cedar::proc::DataRole::Id> slotTypes;
  slotTypes.push_back(cedar::proc::DataRole::BUFFER);
  slotTypes.push_back(cedar::proc::DataRole::OUTPUT);

  for (unsigned int s = 0; s < slotTypes.size(); s++)
  {

    if (mStepToConfigure->hasSlotForRole(slotTypes[s]))
    {
      cedar::proc::Connectable::SlotList dataSlots = mStepToConfigure->getOrderedDataSlots(slotTypes[s]);
      if (dataSlots.size() > 0)
      {
        createRoleSection(cedar::proc::DataRole::type().get(slotTypes[s]).prettyString());
      }

      for (unsigned int i = 0; i < dataSlots.size(); i++)
      {
        auto property = new RecorderProperty(this, mStepToConfigure->getName(), dataSlots[i]);
        mMainLayout->addLayout(property);
        QObject::connect(property, SIGNAL(changed()), this, SIGNAL(settingsChanged()));
      }
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

void cedar::proc::gui::RecorderWidget::createHeader(const std::string& name)
{  
  QHBoxLayout* step_name_layout = new QHBoxLayout();
  //Create step name.
  QLabel* step_name = new QLabel(QString(name.c_str()));
  QFont step_name_font= step_name->font();
  step_name_font.setPointSize(step_name_font.pointSize() + 2);
  step_name_font.setBold(true);
  step_name->setFont(step_name_font);
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

void cedar::proc::gui::RecorderWidget::updateName()
{
  if (cedar::proc::Step* pStep = dynamic_cast<cedar::proc::Step*>(QObject::sender()))
  {
    //rename registered Data
    std::vector<cedar::proc::DataRole::Id> slotTypes;
    slotTypes.push_back(cedar::proc::DataRole::BUFFER);
    slotTypes.push_back(cedar::proc::DataRole::OUTPUT);

    for (unsigned int s = 0; s < slotTypes.size(); s++)
    {
      if (pStep->hasSlotForRole(slotTypes[s]))
      {
        const cedar::proc::Connectable::SlotList dataSlots = pStep->getOrderedDataSlots(slotTypes[s]);
        for (unsigned int i = 0; i < dataSlots.size(); ++i)
        {
          if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(dataSlots[i]->getData()))
          {
            cedar::aux::RecorderSingleton::getInstance()->renameRegisteredData
                                                          (
                                                              dataSlots[i]->getData(),
                                                            pStep->getName() + "_" + dataSlots[i]->getName()
                                                          );
          }
        }
      }

    }

    this->refreshWidget();
  }
}

void cedar::proc::gui::RecorderWidget::emitStepRegisteredinRecorder()
{
  emit stepRegisteredinRecorder();
}
