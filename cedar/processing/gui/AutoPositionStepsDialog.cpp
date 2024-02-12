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

    File:        AutoPositionStepsDialog.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 09 12

    Description: Source file for the class cedar::proc::gui::AutoPositionStepsDialog.

    Credits:

======================================================================================================================*/

#define BOOST_TIMER_ENABLE_DEPRECATED

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/AutoPositionStepsDialog.h"

// CEDAR INCLUDES
//#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/processing/gui/layout/ForceDirectedLayout.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/shared_ptr.hpp>
#endif
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::AutoPositionStepsDialog::AutoPositionStepsDialog(QWidget *pParent)
:
QDialog(pParent),
mpAttractionForceParameter(new cedar::aux::DoubleParameter()),
liveUpdate(false)
{
  this->setupUi(this);
  this->mpForceDirectedLayout = boost::shared_ptr<cedar::proc::gui::layout::ForceDirectedLayout>(
    new cedar::proc::gui::layout::ForceDirectedLayout());
  QObject::connect(this->mpResetBtn, SIGNAL(clicked()), this, SLOT(reset()));
  QObject::connect(this->mpArrangeBtn, SIGNAL(clicked()), this, SLOT(arrange()));
  QObject::connect(this->mpLiveUpdate, SIGNAL(toggled(bool)), this, SLOT(liveUpdateChecked(bool)));


  QObject::connect(this->mpAttractionForce, SIGNAL(parameterPointerChanged()), this,
                   SLOT(attractionForceParameterPointerChanged()));
  this->mpAttractionForce->setParameter(this->mpAttractionForceParameter);
  if(auto doubleParameter = dynamic_cast<cedar::aux::NumericParameter<double>*>(this->mpAttractionForce->getParameter().get()))
  {
    doubleParameter->setValue(this->mpForceDirectedLayout->getAttractionForceFactor());
  }

}

cedar::proc::gui::AutoPositionStepsDialog::~AutoPositionStepsDialog()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::AutoPositionStepsDialog::setGroup(cedar::proc::gui::GroupPtr pGroup)
{
  this->mpGroup = pGroup;
  this->mpForceDirectedLayout->setGroup(pGroup.get());
}

void cedar::proc::gui::AutoPositionStepsDialog::reset()
{
  this->mpForceDirectedLayout->reset();
  this->mpResetBtn->setEnabled(false);
}

void cedar::proc::gui::AutoPositionStepsDialog::arrange()
{
  if(this->mpGroup != nullptr)
  {
    this->mpForceDirectedLayout->arrange();
    this->mpResetBtn->setEnabled(true);
  }
}

void cedar::proc::gui::AutoPositionStepsDialog::liveUpdateChecked(bool toggled)
{
  this->liveUpdate = toggled;
  if(toggled)
  {
    this->arrange();
  }
}

void cedar::proc::gui::AutoPositionStepsDialog::attractionForceValueChanged(){
  if(auto doubleParameter = dynamic_cast<cedar::aux::NumericParameter<double>*>(this->mpAttractionForce->getParameter().get()))
  {
    this->mpForceDirectedLayout->setAttractionForceFactor(doubleParameter->getValue());
  }
  if(this->liveUpdate)
  {
    this->arrange();
  }
}

void cedar::proc::gui::AutoPositionStepsDialog::attractionForceParameterPointerChanged(){
  QObject::connect(this->mpAttractionForce->getParameter().get(), SIGNAL(valueChanged()),
                   this, SLOT(attractionForceValueChanged()));
}
