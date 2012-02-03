/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        RigidBodyWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::aux::RigidBodyWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/RigidBodyWidget.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include "stdio.h"
#include <iostream>
#include <QtGui/QApplication>
#include <QtGui/QLabel>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::aux::gui::RigidBodyWidget::RigidBodyWidget
(
  const cedar::aux::ObjectPtr &rigidBody,
  QWidget* parent,
  Qt::WindowFlags f
)
:
mpRigidBody(rigidBody),
QWidget(parent, f)
{
  // store a smart pointer to KinematicChain
  mDecimals = 2;

  initWindow();
  return;
}


cedar::aux::gui::RigidBodyWidget::RigidBodyWidget
(
  const cedar::aux::ObjectPtr &rigidBody,
  const std::string& configFileName,
  QWidget *parent,
  Qt::WindowFlags
)
:
mpRigidBody(rigidBody),
QWidget(parent),
cedar::aux::ConfigurationInterface(configFileName)
{
  // store a smart pointer to KinematicChain
  mDecimals = 2;

  //
  // read configuration file
  //

  if(addParameter(&mDecimals, "kinematicChainWidgetDecimals", 2) != CONFIG_SUCCESS)
  {
    std::cout << "RigidBodyWidget: Error reading 'kinematicChainWidgetDecimals' from config file!" << std::endl;
  }

  readOrDefaultConfiguration();

  initWindow();
  return;
}

cedar::aux::gui::RigidBodyWidget::~RigidBodyWidget()
{

}


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::gui::RigidBodyWidget::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::aux::gui::RigidBodyWidget::initWindow()
{
  setWindowTitle(QApplication::translate("RigidBodyWidget", "Rigid Body"));

  mpGridLayout = new QGridLayout();

  // add labels
  QLabel* label;
  label = new QLabel(QString("x"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 1);
  label = new QLabel(QString("y"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 2);
  label = new QLabel(QString("z"));
  label->setAlignment(Qt::AlignCenter);
  mpGridLayout->addWidget(label, 0, 3);
  label = new QLabel(QString("pos"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 0, 3);
  label = new QLabel(QString("rot"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 0, 3);
  label = new QLabel(QString("R"));
  label->setAlignment(Qt::AlignRight);
  mpGridLayout->addWidget(label, 0, 3);

  for(unsigned int i = 3; i < 6; i++)
  {
    for(unsigned int j = 1; j < 4; j++)
    {
      // add R label
      QLabel* p_label = new QLabel;
      p_label->setAlignment(Qt::AlignCenter);
      p_label->setText("0.00");
      mpGridLayout->addWidget(p_label, i, j);
    }
  }

  setLayout(mpGridLayout);
  setMaximumHeight(0);

  // start a timer to update the interface
  startTimer(mUpdateInterval);

  return;
}

void cedar::aux::gui::RigidBodyWidget::update()
{

}

