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

    File:        KinematicChainCommandWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Implementation of the @em cedar::dev::KinematicChainCommandWidget class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/KinematicChainCommandWidget.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------

cedar::dev::gui::KinematicChainCommandWidget::KinematicChainCommandWidget
(
  cedar::dev::KinematicChainPtr kinematicChain,
  QWidget * parent,
  Qt::WindowFlags f
)
:
QWidget(parent, f),
mpKinematicChain(kinematicChain),
mTimerId(0)
{
  initWindow();
}

cedar::dev::gui::KinematicChainCommandWidget::~KinematicChainCommandWidget()
{

}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::gui::KinematicChainCommandWidget::timerEvent(QTimerEvent*)
{
  commandJoints();
}

void cedar::dev::gui::KinematicChainCommandWidget::setDecimals(unsigned int decimals)
{
  for(unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mCommandBoxes[j]->setDecimals(decimals);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::setSingleStep(double singleStep)
{
  for(unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mCommandBoxes[j]->setSingleStep(singleStep);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::changeWorkingMode(int mode)
{
  // we have to reset the used command in here
  mpModeBox->setCurrentIndex(mode);
  mpKinematicChain->clearAllCommands();
  update();
}

void cedar::dev::gui::KinematicChainCommandWidget::commandJoints()
{  
  mpKinematicChain->clearAllCommands();

  // should be supressed, un-lock:
  if (mpKinematicChain->getSuppressUserSideInteraction())
  {
    mpKinematicChain->setSuppressUserSideInteraction(false);
  }

  if (!mpKinematicChain->isCommunicating())
  {
    cedar::aux::LogSingleton::getInstance()->message(
      mpKinematicChain->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  auto command_type = mpKinematicChain->getCommandTypeForName(mpModeBox->currentText().toStdString());
  auto mat_data = cv::Mat(mpKinematicChain->getNumberOfJoints(), 1, CV_32F);

  for(unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    mat_data.at<float>(i, 0) = float(mCommandBoxes[i]->value());
  }

  mpKinematicChain->setUserSideCommandBuffer(command_type, mat_data);  
}

void cedar::dev::gui::KinematicChainCommandWidget::stopMovement()
{
  if (!mpKinematicChain->isCommunicating())
  {
    cedar::aux::LogSingleton::getInstance()->message(
      mpKinematicChain->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  mpKeepMovingBox->setChecked(false);
  mpKinematicChain->startBrakingSlowly();

#if 0
  mpKinematicChain->clearUserCommand();
  for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
  {
    mpKinematicChain->setJointVelocity(j, 0);
  }
#endif  
  update();
  //mpKinematicChain->clearUserCommand();
}

void cedar::dev::gui::KinematicChainCommandWidget::saveInitialConfiguration()
{
  const std::string filename = "initial_configurations_"+ mpKinematicChain->getName() + ".json";
  cedar::aux::Path file_path = cedar::aux::Path::globalCofigurationBaseDirectory() + filename;
  cedar::aux::ConfigurationNode root, joints;

  try
  {
    boost::property_tree::read_json(file_path.toString(), root);
  }catch(...)
  {
    // file has not been initialized yet... should be no problem
  }

  // serialized joint angles
  for(unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    joints.put(std::to_string(i), float(mCommandBoxes[i]->value()));
  }

  const std::string& conf_name = mpIniconfName->text().toStdString();

  const boost::property_tree::ptree::iterator found = root.to_iterator(root.find(conf_name));
  if( found != root.end())
  {
    root.erase(found); // overwrite existing configuration of that name
  }

  root.push_back(cedar::aux::ConfigurationNode::value_type(conf_name, joints));
  boost::property_tree::write_json(file_path.toString(), root);

  mpKinematicChain->readInitialConfigurations();
  loadInitialConfigurations();
}
void cedar::dev::gui::KinematicChainCommandWidget::update()
{
  // update command boxes
  CEDAR_DEBUG_ASSERT(mpKinematicChain->getNumberOfJoints() == mCommandBoxes.size());
  switch(mpModeBox->currentIndex())
  {
  case 0:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointAngle(j));
      mCommandBoxes[j]->blockSignals(false);
    }
    break;
  case 1:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointVelocity(j));
      mCommandBoxes[j]->blockSignals(false);
    }
    break;

  case 2:
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); ++j)
    {
      mCommandBoxes[j]->blockSignals(true);
      mCommandBoxes[j]->setValue(mpKinematicChain->getJointAcceleration(j));
      mCommandBoxes[j]->blockSignals(false);
    }
  case 3: // STOP
    break;
  default:
    CEDAR_THROW(cedar::aux::UnhandledValueException, "This is not a handled case.");
  }
}
void cedar::dev::gui::KinematicChainCommandWidget::setKeepSendingState(int state)
{
  if (state)
  {
    if (mTimerId)
    {
      killTimer(mTimerId);
    }
    mTimerId = startTimer(mUpdateInterval);
  }
  else
  {
    killTimer(mTimerId);
  }
}
void cedar::dev::gui::KinematicChainCommandWidget::initWindow()
{
  setWindowTitle(QApplication::translate("KinematicChainWindow", "Command"));

  mpGridLayout = new QGridLayout();

  QFont font;
  font.setBold(true);

  QLabel* com_label = new QLabel(QApplication::translate("KinematicChainWindow", "Commands"));
  com_label->setFont(font);
  mpGridLayout->addWidget(com_label, 0, 0);

  // mode selection
  QLabel* mode_label = new QLabel(QApplication::translate("KinematicChainWindow", "operate on:"));
  mode_label->setAlignment(Qt::AlignLeft);
  mpGridLayout->addWidget(mode_label, 1, 0);
  mpModeBox = new QComboBox();
  mpModeBox->addItem(QString("Joint Angles"));
  mpModeBox->addItem(QString("Joint Velocities"));
  mpModeBox->addItem(QString("Joint Accelerations"));

  mpModeBox->setCurrentIndex(0);

  mpGridLayout->addWidget(mpModeBox, 2, 0);
  connect(mpModeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeWorkingMode(int)));

  // move button
  QPushButton* move_button = new QPushButton(QApplication::translate("KinematicChainWindow", "send"));
  mpGridLayout->addWidget(move_button, 3, 0);
  connect(move_button, SIGNAL(pressed()), this, SLOT(commandJoints()));

  // copy button
  QPushButton* copy_button = new QPushButton(QApplication::translate("KinematicChainWindow", "copy"));
  mpGridLayout->addWidget(copy_button, 4, 0);
  connect(copy_button, SIGNAL(pressed()), this, SLOT(update()));

  // stop button
  QPushButton* stop_button = new QPushButton(QApplication::translate("KinematicChainWindow", "brake now!"));
  mpGridLayout->addWidget(stop_button, 5, 0);
  connect(stop_button, SIGNAL(pressed()), this, SLOT(stopMovement()));

  // keep moving
  mpKeepMovingBox = new QCheckBox(QApplication::translate("KinematicChainWindow", "keep sending"));
  mpGridLayout->addWidget(mpKeepMovingBox, 6, 0);
  connect(mpKeepMovingBox, SIGNAL(stateChanged(int)), this, SLOT(setKeepSendingState(int)));

  for (unsigned int i = 0; i < mpKinematicChain->getNumberOfJoints(); ++i)
  {
    // add label
    char labelText[10];
    sprintf(labelText, "Joint %d", i);
    QLabel *label = new QLabel(QApplication::translate("KinematicChainWindow", labelText));
    label->setAlignment(Qt::AlignRight);
    mpGridLayout->addWidget(label, i, 1);

    // add spinboxes
    QDoubleSpinBox* command_box = new QDoubleSpinBox();
    command_box->setRange(-999999.0, 999999.0);
    command_box->setValue(0.0);
    command_box->setDecimals(6);
    command_box->setSingleStep(0.01);
    mpGridLayout->addWidget(command_box, i, 2);
    mCommandBoxes.push_back(command_box);
  }

  update();

  // horizontal seperator
  QFrame* seperator = new QFrame();
  seperator->setFrameShape(QFrame::HLine);
  seperator->setFrameShadow(QFrame::Sunken);
  mpGridLayout->addWidget(seperator, 7, 0, 1, 3, Qt::AlignTop);

  // initial configuration label
  QLabel* iniconf_label = new QLabel(QApplication::translate("KinematicChainWindow", "Initial Configurations"));
  iniconf_label->setAlignment(Qt::AlignLeft);
  iniconf_label->setFont(font);
  mpGridLayout->addWidget(iniconf_label, 8, 0);

  // initial configuration screen controls
  mpIniconfName = new QLineEdit("configuration name");
  mpGridLayout->addWidget(mpIniconfName, 9, 0);

  QPushButton* iniconf_button = new QPushButton(QApplication::translate("KinematicChainWindow", "Save above values as initial configuration"));
  mpGridLayout->addWidget(iniconf_button, 9, 1, 1, 2);
  connect(iniconf_button, SIGNAL(pressed()), this, SLOT(saveInitialConfiguration()));

  mpIniconfBox = new QComboBox();
  loadInitialConfigurations();
  changeInitialConfig();

  mpGridLayout->addWidget(mpIniconfBox, 10, 0);
  connect(mpIniconfBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeInitialConfig()));

  // apply button
  QPushButton* apply_button = new QPushButton(QApplication::translate("KinematicChainWindow", "apply"));
  apply_button->setFixedWidth(150);
  mpGridLayout->addWidget(apply_button, 10, 1);
  connect(apply_button, SIGNAL(pressed()), this, SLOT(applyInitialConfig()));

  // delete button
  QPushButton* del_button = new QPushButton(QApplication::translate("KinematicChainWindow", "delete"));
  del_button->setFixedWidth(150);
  mpGridLayout->addWidget(del_button, 10, 2);
  connect(del_button, SIGNAL(pressed()), this, SLOT(deleteInitialConfig()));

  setLayout(mpGridLayout);
}

void cedar::dev::gui::KinematicChainCommandWidget::loadInitialConfigurations()
{
  mpIniconfBox->clear();

  std::vector<std::string> lConfNames = mpKinematicChain->getInitialConfigurationNames();
  std::string lCurConf = mpKinematicChain->getCurrentInitialConfigurationName();

  for (const std::string &confName : lConfNames)
  {
    mpIniconfBox->addItem(QString::fromStdString(confName));
  }

  mpIniconfBox->setCurrentIndex(mpIniconfBox->findText(QString::fromStdString(lCurConf)));
}

void cedar::dev::gui::KinematicChainCommandWidget::changeInitialConfig()
{
  //Does this in any case need the info from the jsonFile?

//  const std::string filename = "initial_configurations_"+ mpKinematicChain->getName() + ".json";
//  cedar::aux::Path file_path = cedar::aux::Path::globalCofigurationBaseDirectory() + filename;
//
//  cedar::aux::ConfigurationNode root;
//
//  try
//  {
//    boost::property_tree::read_json(file_path.toString(), root);
//  }catch(...)
//  {
//    return;
//  }

  const std::string& conf_name = mpIniconfBox->currentText().toStdString();
//  const boost::property_tree::ptree::iterator found = root.to_iterator(root.find(conf_name));

  if( mpKinematicChain->hasInitialConfiguration(conf_name))
  {
    mpKinematicChain->setCurrentInitialConfiguration(conf_name);

    for (uint i=0; i<mpKinematicChain->getNumberOfJoints(); ++i)
    {
      float angle = mpKinematicChain->getInitialConfiguration(conf_name).at<float>(i,0);
      mCommandBoxes[i]->setValue(angle);
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error(
            "The configuration: " +conf_name+" is not defined for: " +mpKinematicChain->prettifyName() + ". This is not supposed to happen!",
            CEDAR_CURRENT_FUNCTION_NAME);
  }
}

void cedar::dev::gui::KinematicChainCommandWidget::applyInitialConfig()
{
  if (!mpKinematicChain->isCommunicating())
  {
    cedar::aux::LogSingleton::getInstance()->message(
      mpKinematicChain->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  mpKinematicChain->applyInitialConfiguration(mpIniconfBox->currentText().toStdString());
}

void cedar::dev::gui::KinematicChainCommandWidget::deleteInitialConfig()
{

  const QString conf_name = mpIniconfBox->currentText();

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Please confirm deletion", "Are you sure to delete "+conf_name+" from the list of initial configurations?",
                                   QMessageBox::Yes|QMessageBox::No);

  if (reply == QMessageBox::No)
  {
    return;
  }

  const std::string filename = "initial_configurations_"+ mpKinematicChain->getName() + ".json";
  cedar::aux::Path file_path = cedar::aux::Path::globalCofigurationBaseDirectory() + filename;
  cedar::aux::ConfigurationNode root;

  try
  {
    mpKinematicChain->deleteInitialConfiguration(conf_name.toStdString());

    boost::property_tree::read_json(file_path.toString(), root);
    const boost::property_tree::ptree::iterator found = root.to_iterator(root.find(mpIniconfBox->currentText().toStdString()));

    if( found != root.end())
    {
      root.erase(found); // overwrite existing configuration of that name
    }

    boost::property_tree::write_json(file_path.toString(), root);
    loadInitialConfigurations();

  }catch(...)
  {
    return;
  }
}
