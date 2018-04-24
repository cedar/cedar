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

    File:        FriStatusWidget.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/gui/FriStatusWidget.h"

// SYSTEM INCLUDES
#include <sstream>
#ifdef DEBUG
#include <iostream>
#endif
#include <string>
#include <fri/friremote.h>
#include <boost/lexical_cast.hpp>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::gui::FriStatusWidget::FriStatusWidget(cedar::dev::kuka::FRIChannelPtr pKukaIn, QWidget *parent)
:
cedar::aux::gui::BaseWidget("FriStatusWidget", parent)
{
  mIsInit = false;
  mpKukaIn = pKukaIn;

  setupUi(this);
  init();
}

cedar::dev::kuka::gui::FriStatusWidget::~FriStatusWidget()
{
  //nothing yet
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kuka::gui::FriStatusWidget::init()
{
  updateInformation();
  mIsInit = true;
}

void cedar::dev::kuka::gui::FriStatusWidget::updateInformation()
{
  // Translations from status-/quality-values to text
  std::string quality_names[]
    = {"BAD (0)", "UNACCEPTABLE (1)", "GOOD (2)", "PERFECT (3)"};
  std::string state_names[] = {"OFF (0)", "Monitor Mode", "Command Mode"};

  // set Text for Fri state and Fri quality
  mpLabelStateData->setText(state_names[mpKukaIn->getFriState()].c_str());
  mpLabelQualData->setText(quality_names[mpKukaIn->getFriQuality()].c_str());
  // set text for power status
  if (mpKukaIn->isDrivesPowerOn())
  {
    mpLabelPowerData->setText("ON");
  }
  else
  {
    mpLabelPowerData->setText("OFF");
  }
  // set text for sample time
  std::stringstream s;
  s << mpKukaIn->getSampleTime() << "s";
  mpLabelSampleTimeData->setText(s.str().c_str());

  std::string control_desc= "?";
  switch( mpKukaIn->getControlScheme() )
  {
    case FRI_CTRL_POSITION:
      control_desc= "POSITION (1)";
      break;
    case FRI_CTRL_CART_IMP:
      control_desc= "CARTESIAN IMPEDANCE (2)";
      break;
    case FRI_CTRL_JNT_IMP:
      control_desc= "JOINT IMPEDANCE (3)";
      break;
    case FRI_CTRL_OTHER:
      control_desc= "OTHER (3)";
      break;
    default: 
      control_desc= "UKNOWN (" + 
        boost::lexical_cast<std::string>(mpKukaIn->getControlScheme()) 
          + ")";
      break;
  }

  mpLabelControlSchemeData->setText( control_desc.c_str() );
}

void cedar::dev::kuka::gui::FriStatusWidget::timerEvent(QTimerEvent* /* pEvent */)
{
  //set the displayed data
  updateInformation();
}
#endif // CEDAR_USE_KUKA_FRI
