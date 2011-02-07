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

    File:        FriStatusWidget.cpp

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2011 01 25

    Description: implementation of the widget that shows information about the status of the Kuka FRI

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "FriStatusWidget.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <sstream>
#ifdef DEBUG
#include <iostream>
#endif

using namespace cedar::dev::robot::kuka::gui;
using namespace cedar::dev::robot::kuka;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
FriStatusWidget::FriStatusWidget(cedar::dev::robot::kuka::KukaInterfacePtr &pKukaIn, QWidget *parent)
{
  mIsInit = false;
  mpKukaIn = pKukaIn;

  setupUi(this);
  init();
}

FriStatusWidget::~FriStatusWidget()
{
  //nothing yet
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void FriStatusWidget::init()
{
  updateInformation();
  mIsInit = true;
}

void FriStatusWidget::updateInformation()
{
  //Translations from status-/quality-values to text
  string quality_names[] = { string("BAD (0)"), string("UNACCEPTABLE (1)"), string("GOOD (2)"), string("PERFECT (3)") };
  string state_names[] = { string("OFF (0)"), string("Monitor Mode"), string("Command Mode") };
  //set Text for Fri state and Fri quality
  mpLabelStateData->setText(state_names[mpKukaIn->getFriState()].c_str());
  mpLabelQualData->setText(quality_names[mpKukaIn->getFriQuality()].c_str());
  //set text for power status
  if (mpKukaIn->isPowerOn())
  {
    mpLabelPowerData->setText("ON");
  }
  else
  {
    mpLabelPowerData->setText("OFF");
  }
  //set text for sample time
  stringstream s;
  s <<mpKukaIn->getSampleTime()<<"s";
  mpLabelSampleTimeData->setText(s.str().c_str());
}

void FriStatusWidget::timerEvent(QTimerEvent* pEvent)
{
  //set the displayed data
  updateInformation();
}
