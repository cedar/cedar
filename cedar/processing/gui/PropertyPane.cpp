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

    File:        PropertyPane.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/PropertyPane.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/processing/gui/ProjectionMappingParameter.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/gui/BoolParameter.h"
#include "cedar/auxiliaries/gui/DoubleParameter.h"
#include "cedar/auxiliaries/gui/DoubleVectorParameter.h"
#include "cedar/auxiliaries/gui/EnumParameter.h"
#include "cedar/auxiliaries/gui/FileParameter.h"
#include "cedar/auxiliaries/gui/StringParameter.h"
#include "cedar/auxiliaries/gui/UIntParameter.h"
#include "cedar/auxiliaries/gui/UIntVectorParameter.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/gui/DirectoryParameter.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QApplication>
#include <boost/bind.hpp>


cedar::proc::gui::PropertyPane::DataWidgetTypes cedar::proc::gui::PropertyPane::mDataWidgetTypes;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
QTableWidget(pParent)
{
  this->setSelectionMode(QAbstractItemView::NoSelection);
  this->setTabKeyNavigation(false);
}

cedar::proc::gui::PropertyPane::~PropertyPane()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PropertyPane::resetContents()
{
  if (mSlotConnection.connected())
  {
    mSlotConnection.disconnect();
  }

  // disconnect all signals from the configurable
  cedar::aux::ConfigurablePtr configurable = this->mDisplayedConfigurable.lock();
  if (configurable)
  {
    this->disconnect(configurable);
  }

  this->clearContents();
  this->setRowCount(0);
  this->mParameterWidgetRowIndex.clear();
  this->mParameterRowIndex.clear();
}

void cedar::proc::gui::PropertyPane::disconnect(cedar::aux::ConfigurablePtr pConfigurable)
{
  for (cedar::aux::Configurable::ParameterList::iterator iter = pConfigurable->getParameters().begin();
      iter != pConfigurable->getParameters().end();
      ++iter)
  {
    // disconnect everything between the parameter and this
    QObject::disconnect(iter->get(), 0, this, 0);
  }

  for (cedar::aux::Configurable::Children::const_iterator iter = pConfigurable->configurableChildren().begin();
       iter != pConfigurable->configurableChildren().end();
       ++iter)
  {
    this->disconnect(iter->second);
  }
}

void cedar::proc::gui::PropertyPane::display(cedar::proc::StepPtr pStep)
{
  this->resetContents();

  std::string label = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(pStep)->getClassId();
  this->addLabelRow(label);
  this->mDisplayedConfigurable = pStep;
  this->display(cedar::aux::ConfigurablePtr(this->mDisplayedConfigurable), false);
}

void cedar::proc::gui::PropertyPane::display(cedar::proc::TriggerPtr pTrigger)
{
  this->resetContents();

  std::string label
    = cedar::proc::DeclarationRegistrySingleton::getInstance()->getDeclarationOf(pTrigger)->getClassId();
  this->addLabelRow(label);
  this->mDisplayedConfigurable = pTrigger;
  this->display(cedar::aux::ConfigurablePtr(this->mDisplayedConfigurable), false);
}

void cedar::proc::gui::PropertyPane::display(cedar::aux::ConfigurablePtr pConfigurable, bool reset)
{
  if (reset)
  {
    this->resetContents();
  }

  this->append(pConfigurable->getParameters());
  mSlotConnection
    = pConfigurable->connectToTreeChangedSignal(boost::bind(&cedar::proc::gui::PropertyPane::redraw, this));
  for (cedar::aux::Configurable::Children::const_iterator iter = pConfigurable->configurableChildren().begin();
       iter != pConfigurable->configurableChildren().end();
       ++iter)
  {
    this->addHeadingRow(iter->first);
    this->append(iter->second->getParameters());
  }
}

void cedar::proc::gui::PropertyPane::addHeadingRow(const std::string& label)
{
  int row = this->rowCount();
  this->insertRow(row);
  QLabel *p_label = new QLabel();

  QFont font = p_label->font();
  font.setBold(true);
  font.setPointSize(font.pointSize() + 1);
  p_label->setFont(font);

  p_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  p_label->setText(label.c_str());
  p_label->setAutoFillBackground(true);
  p_label->setBackgroundRole(QPalette::Dark);
  p_label->setForegroundRole(QPalette::Light);
  this->setCellWidget(row, 0, p_label);
  this->setSpan(row, 0, 1, 2);
}

void cedar::proc::gui::PropertyPane::addLabelRow(const std::string& label)
{
  int row = this->rowCount();
  this->insertRow(row);
  QLabel *p_label = new QLabel();
  p_label->setText(label.c_str());
  this->setCellWidget(row, 0, p_label);
  this->setSpan(row, 0, 1, 2);
}

void cedar::proc::gui::PropertyPane::append(cedar::aux::Configurable::ParameterList& parameters)
{
  for (cedar::aux::Configurable::ParameterList::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
  {
    this->addPropertyRow(*iter);
  }
}

void cedar::proc::gui::PropertyPane::indicateChange(QLabel *pLabel, bool changed)
{
  QFont font = pLabel->font();
  font.setBold(changed);
  pLabel->setFont(font);
}

void cedar::proc::gui::PropertyPane::addPropertyRow(cedar::aux::ParameterPtr parameter)
{
  if (!parameter->isHidden())
  {
    int row = this->rowCount();
    this->insertRow(row);
    QLabel *p_label = new QLabel();
    p_label->setText(parameter->getName().c_str());
    this->setCellWidget(row, 0, p_label);

    this->indicateChange(p_label, parameter->isChanged());

    cedar::aux::gui::Parameter *p_widget = dataWidgetTypes().get(parameter)->allocateRaw();
    p_widget->setParent(this);
    p_widget->setParameter(parameter);
    p_widget->setEnabled(!parameter->isConstant());
    this->setCellWidget(row, 1, p_widget);
    this->resizeRowToContents(row);

    this->mParameterWidgetRowIndex[p_widget] = row;
    this->mParameterRowIndex[parameter.get()] = row;
    QObject::connect(p_widget, SIGNAL(heightChanged()), this, SLOT(rowSizeChanged()));
    QObject::connect(parameter.get(), SIGNAL(changedFlagChanged()), this, SLOT(parameterChangeFlagChanged()));
  }
}

cedar::proc::gui::PropertyPane::DataWidgetTypes& cedar::proc::gui::PropertyPane::dataWidgetTypes()
{
  if (cedar::proc::gui::PropertyPane::mDataWidgetTypes.empty())
  {
    // parameter types in auxiliaries
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::DoubleParameter,
      cedar::aux::gui::DoubleParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::UIntParameter, cedar::aux::gui::UIntParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::StringParameter,
      cedar::aux::gui::StringParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::BoolParameter,
      cedar::aux::gui::BoolParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::DoubleVectorParameter,
      cedar::aux::gui::DoubleVectorParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::UIntVectorParameter,
      cedar::aux::gui::UIntVectorParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::DirectoryParameter,
      cedar::aux::gui::DirectoryParameter
    >
    ();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::EnumParameter, cedar::aux::gui::EnumParameter>();

    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::aux::FileParameter,
      cedar::aux::gui::FileParameter
    >
    ();

    // parameter types in processing
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add
    <
      cedar::proc::ProjectionMappingParameter,
      cedar::proc::gui::ProjectionMappingParameter
    >
    ();
  }
  return cedar::proc::gui::PropertyPane::mDataWidgetTypes;
}

int cedar::proc::gui::PropertyPane::getSenderParameterRowWidget() const
{
  cedar::aux::gui::Parameter *p_parameter = dynamic_cast<cedar::aux::gui::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  CEDAR_DEBUG_ASSERT(this->mParameterWidgetRowIndex.find(p_parameter) != this->mParameterWidgetRowIndex.end());
  return this->mParameterWidgetRowIndex.find(p_parameter)->second;
}

int cedar::proc::gui::PropertyPane::getSenderParameterRow() const
{
  cedar::aux::Parameter *p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  CEDAR_DEBUG_ASSERT(this->mParameterRowIndex.find(p_parameter) != this->mParameterRowIndex.end());
  return this->mParameterRowIndex.find(p_parameter)->second;
}

void cedar::proc::gui::PropertyPane::parameterChangeFlagChanged()
{
  cedar::aux::Parameter *p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  int row = this->getSenderParameterRow();

  QLabel *p_label = dynamic_cast<QLabel*>(this->cellWidget(row, 0)); // column 0 contains the label
  CEDAR_DEBUG_ASSERT(p_label != NULL);

  this->indicateChange(p_label, p_parameter->isChanged());
}

void cedar::proc::gui::PropertyPane::rowSizeChanged()
{
  int row = this->getSenderParameterRowWidget();

  // the process-events call is only necessary because qt does otherwise not detect the new size properly.
  // should this bug ever be fixed, this can be removed.
  QApplication::processEvents();

  this->resizeRowToContents(row);
}

void cedar::proc::gui::PropertyPane::resetPointer()
{
  this->mDisplayedConfigurable.reset();
  this->clearContents();
  this->setRowCount(0);
}

void cedar::proc::gui::PropertyPane::redraw()
{
  this->resetContents();
  this->display(cedar::aux::ConfigurablePtr(this->mDisplayedConfigurable));
}
