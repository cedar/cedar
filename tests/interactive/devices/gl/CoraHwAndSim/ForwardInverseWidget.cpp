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

    File:        ForwardInverseWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 04 28

    Description: Widget to control CoRA via forward and reverse kinematic.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "ForwardInverseWidget.h"

// PROJECT INCLUDES

#include <cedar/auxiliaries/exceptions.h>

// SYSTEM INCLUDES

#include <QApplication>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QRadioButton>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

ForwardInverseWidget::ForwardInverseWidget
(
  const cedar::dev::robot::KinematicChainPtr kinematicChain,
  cedar::dev::robot::KinematicChainModelPtr kinematicChainModel,
  QWidget* parent, Qt::WindowFlags f
)
:
QWidget(parent, f)
{
  // store a smart pointer to KinematicChain
  mpKinematicChains.push_back(kinematicChain);
  mpKinematicChainModel = kinematicChainModel;
  mDecimals = 3;
  mSingleStep = 0.01;
  mpClosedFormInverseKinematics = new ClosedFormInverseKinematics();
  mpClosedFormInverseKinematics->InitVariables();

  initWindow();
  return;
}


ForwardInverseWidget::ForwardInverseWidget
(
  const std::vector<cedar::dev::robot::KinematicChainPtr> &kinematicChains,
  cedar::dev::robot::KinematicChainModelPtr kinematicChainModel,
  QWidget *parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f)
{
  for(unsigned int i = 1; i < kinematicChains.size(); ++i)
  {
    // ideally, the reference geometry behind all the chains should be the same
    // here, at least we make sure that the number of joints is the same
    if(kinematicChains[i]->getNumberOfJoints() != kinematicChains[0]->getNumberOfJoints())
    {
      std::cout << "KinematicChainWidget: Error, kinematic chains do not have the same number of joints!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Kinematic chains do not have the same number of joints!");
    }
  }

  // store smart pointers to KinematicChains
  mpKinematicChains = kinematicChains;
  mpKinematicChainModel = kinematicChainModel;
  mDecimals = 3;
  mSingleStep = 0.01;
  mpClosedFormInverseKinematics = new ClosedFormInverseKinematics();
  mpClosedFormInverseKinematics->InitVariables();

  initWindow();
  return;
}


ForwardInverseWidget::~ForwardInverseWidget()
{
  for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
  {
    mpKinematicChains[i]->stop();
  }
  mpTimer->stop();
  delete mpTimer;
  delete mpClosedFormInverseKinematics;
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void ForwardInverseWidget::setActiveColumn(unsigned int c)
{

  // enable/disable column with joints
  bool enabled = (c == 0) ? true : false;
  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    mpGridLayout->itemAtPosition(i+1, 1)->widget()->setEnabled(enabled);
  }

  // enable/disable column with task coordinates
  enabled = (c == 1) ? true : false;
  for(unsigned int i = 0; i < 7; ++i)
  {
    mpGridLayout->itemAtPosition(i+1, 3)->widget()->setEnabled(enabled);
  }

  return;
}


void ForwardInverseWidget::radioButtonForwardClicked()
{
  setActiveColumn(0);
  updateSpinBoxes();
}


void ForwardInverseWidget::radioButtonInverseClicked()
{
  setActiveColumn(1);
  updateSpinBoxes();
}


void ForwardInverseWidget::updateSpinBoxes()
{

  // update joint positions

  for(unsigned i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    QDoubleSpinBox *p_spin_box = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(i+1, 1)->widget());

    if(!p_spin_box->hasFocus())
    {
      p_spin_box->setValue(mpKinematicChains[0]->getJointAngle(i));
    }
  }

  // update task coordinates

  mpKinematicChainModel->update();

  for(unsigned i = 0; i < 3; ++i)
  {
    QDoubleSpinBox *p_spin_box = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(i+1, 3)->widget());

    if(!p_spin_box->hasFocus())
    {
      p_spin_box->setValue(mpKinematicChainModel->calculateEndEffectorPosition().at<double>(i, 0));
    }
  }

  // phi, theta
  // TODO calculate phi and theta

  // gamma
  QDoubleSpinBox *p_spin_box = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(6, 3)->widget());

  if(!p_spin_box->hasFocus())
  {
    p_spin_box->setValue(mpKinematicChains[0]->getJointAngle(7));
  }

  // arm
  // TODO calculate redundancy angle

  return;
}


void ForwardInverseWidget::updateJointValue()
{
  QDoubleSpinBox *sender = static_cast<QDoubleSpinBox*>(this->sender());
  int index_sender = mpGridLayout->indexOf(sender);
  int row, column;
  int dummy1, dummy2;
  double value;
  double x, y, z, phi, theta, gamma, arm;
  QDoubleSpinBox *p_spin_box_x;
  QDoubleSpinBox *p_spin_box_y;
  QDoubleSpinBox *p_spin_box_z;
  QDoubleSpinBox *p_spin_box_p;
  QDoubleSpinBox *p_spin_box_t;
  QDoubleSpinBox *p_spin_box_g;
  QDoubleSpinBox *p_spin_box_a;

  mpGridLayout->getItemPosition(index_sender, &row, &column, &dummy1, &dummy2);

  switch(column)
  {
  case 1:

    value = sender->value();
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAngle(row-1, value);
    }
    break;

  case 3:

    // x, y, z, phi, theta, gamma
    p_spin_box_x = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(1, 3)->widget());
    p_spin_box_y = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(2, 3)->widget());
    p_spin_box_z = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(3, 3)->widget());
    p_spin_box_p = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(4, 3)->widget());
    p_spin_box_t = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(5, 3)->widget());
    p_spin_box_g = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(6, 3)->widget());
    p_spin_box_a = static_cast<QDoubleSpinBox*>(mpGridLayout->itemAtPosition(7, 3)->widget());

    x = p_spin_box_x->value();
    y = p_spin_box_y->value();
    z = p_spin_box_z->value();
    phi = p_spin_box_p->value();
    theta = p_spin_box_t->value();
    gamma = p_spin_box_g->value();
    arm = p_spin_box_a->value();

    mpClosedFormInverseKinematics->mTaskCoordinates.Pos.at<double>(0, 0) = x * 1000;
    mpClosedFormInverseKinematics->mTaskCoordinates.Pos.at<double>(1, 0) = y * 1000;
    mpClosedFormInverseKinematics->mTaskCoordinates.Pos.at<double>(2, 0) = z * 1000;
    // TODO put EEF in moving direction or whatever
    mpClosedFormInverseKinematics->mTaskCoordinates.eefOrientationAngle.at<double>(0, 0) = phi;
    mpClosedFormInverseKinematics->mTaskCoordinates.eefOrientationAngle.at<double>(1, 0) = theta;
    mpClosedFormInverseKinematics->mTaskCoordinates.eefOrientationAngle.at<double>(2, 0) = gamma;
    mpClosedFormInverseKinematics->mTaskCoordinates.redundancyAng = arm;

    mpClosedFormInverseKinematics->InverseKinematics();

    // check if there's a NaN in the result
    for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
    {
      double value = mpClosedFormInverseKinematics->mJointAngle.at<double>(i, 0);
      // value == NaN ?
      if(value != value)
      {
      	std::cout << "Invalid values from inverse kinematics!" << std::endl;
        return;
      }
    }

    // assign values to the joints
    for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints()-1; ++i)
    {
      for(unsigned int j = 0; j < mpKinematicChains.size(); ++j)
      {
        double value = mpClosedFormInverseKinematics->mJointAngle.at<double>(i, 0);
        // this wonderful expression detects NaNs and replaces them by zero B-)
        value = ( value != value ) ? 0.0 : value;
        mpKinematicChains[j]->setJointAngle(i, value);
      }
    }

    // assign gamma
    for(unsigned int i = 0; i < mpKinematicChains.size(); ++i)
    {
      mpKinematicChains[i]->setJointAngle(7, gamma);
    }

    break;

  default:
    std::cout << "Error: I was not able to determine the corresponding working mode for this signal. This should never happen!"
      << std::endl;
  }

  return;
}


void ForwardInverseWidget::initWindow()
{
  setWindowTitle(QApplication::translate("ForwardInverseWindow", "Cora arm"));

  mpGridLayout = new QGridLayout();
  QRadioButton *radioButtonForward = new QRadioButton(QApplication::translate("ForwardInverseWindow", "forward [rad]"));
  QRadioButton *radioButtonInverse = new QRadioButton(QApplication::translate("ForwardInverseWindow", "inverse [rad]"));
  mpGridLayout->addWidget(radioButtonForward, 0, 1);
  mpGridLayout->addWidget(radioButtonInverse, 0, 3);
  connect(radioButtonForward, SIGNAL(clicked()), this, SLOT(radioButtonForwardClicked()));
  connect(radioButtonInverse, SIGNAL(clicked()), this, SLOT(radioButtonInverseClicked()));

  for(unsigned int i = 0; i < mpKinematicChains[0]->getNumberOfJoints(); ++i)
  {
    // add label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    mpGridLayout->addWidget(label, i+1, 0);

    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setRange(-999999.0, 999999.0);
    doubleSpinBox->setValue(0.0);
    doubleSpinBox->setDecimals(mDecimals);
    doubleSpinBox->setSingleStep(mSingleStep);
    mpGridLayout->addWidget(doubleSpinBox, i+1, 1);
    connect(doubleSpinBox, SIGNAL(editingFinished(void)), this, SLOT(updateJointValue(void)));
  }

  std::vector<std::string> labels;
  labels.push_back("x [m]");
  labels.push_back("y [m]");
  labels.push_back("z [m]");
  labels.push_back("phi");
  labels.push_back("theta");
  labels.push_back("gamma");
  labels.push_back("elbow");
  //string string_xyz = string("xyz");
  QLabel *label;

  for(unsigned int i = 0; i < 7; ++i)
  {
    // add label
    //label = new QLabel(QApplication::translate("KinematicChainWindow", string_xyz.substr(i, 1).c_str()));
    label = new QLabel(QApplication::translate("KinematicChainWindow", labels[i].c_str()));
    mpGridLayout->addWidget(label, i+1, 2);

    // add spinbox
    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setRange(-999999.0, 999999.0);
    doubleSpinBox->setValue(0.0);
    doubleSpinBox->setDecimals(mDecimals);
    doubleSpinBox->setSingleStep(mSingleStep);
    mpGridLayout->addWidget(doubleSpinBox, i+1, 3);
    connect(doubleSpinBox, SIGNAL(editingFinished(void)), this, SLOT(updateJointValue(void)));
  }

  mpGridLayout->setColumnStretch(0,1);
  mpGridLayout->setColumnStretch(1,2);
  mpGridLayout->setColumnStretch(2,1);
  mpGridLayout->setColumnStretch(3,2);

  radioButtonForward->click();
  setLayout(mpGridLayout);
  setMaximumHeight(0);

  // start a timer to update the interface
  mpTimer = new QTimer();
  connect(mpTimer, SIGNAL(timeout()), this, SLOT(updateSpinBoxes()));
  mpTimer->start(mUpdateInterval);

  return;
}
