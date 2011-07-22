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

// LOCAL INCLUDES
#include "processing/gui/PropertyPane.h"
#include "processing/Step.h"
#include "auxiliaries/namespace.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/NumericVectorParameter.h"
#include "processing/gui/BoolParameter.h"
#include "processing/gui/DoubleParameter.h"
#include "processing/gui/DoubleVectorParameter.h"
#include "processing/gui/UIntParameter.h"
#include "processing/gui/UIntVectorParameter.h"
#include "processing/gui/StringParameter.h"
#include "processing/Manager.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QApplication>


cedar::proc::gui::PropertyPane::DataWidgetTypes cedar::proc::gui::PropertyPane::mDataWidgetTypes;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
QTableWidget(pParent)
{
}

cedar::proc::gui::PropertyPane::~PropertyPane()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PropertyPane::display(cedar::proc::StepPtr pStep)
{
  this->clearContents();
  this->setRowCount(0);
  std::string label = cedar::proc::Manager::getInstance().steps().getDeclarationOf(pStep)->getClassId();
  this->addLabelRow(label);
  this->append(pStep->getParameters());

  for (cedar::aux::Configurable::Children::const_iterator iter = pStep->configurableChildren().begin();
       iter != pStep->configurableChildren().end();
       ++iter)
  {
    this->addHeadingRow(iter->first);
    this->append(iter->second->getParameters());
  }
  mDisplayedStep = pStep;
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

void cedar::proc::gui::PropertyPane::append(cedar::aux::Configurable::ParameterMap& parameters)
{
  for (cedar::aux::Configurable::ParameterMap::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
  {
    this->addPropertyRow(iter->second);
  }
}

void cedar::proc::gui::PropertyPane::addPropertyRow(cedar::aux::ParameterBasePtr parameter)
{
  if (!parameter->isHidden())
  {
    int row = this->rowCount();
    this->insertRow(row);
    QLabel *p_label = new QLabel();
    p_label->setText(parameter->getName().c_str());
    this->setCellWidget(row, 0, p_label);

    cedar::proc::gui::ParameterBase *p_widget = dataWidgetTypes().get(parameter)->allocateRaw();
    p_widget->setParent(this);
    p_widget->setParameter(parameter);
    p_widget->setEnabled(!parameter->isConstant());
    this->setCellWidget(row, 1, p_widget);
    this->resizeRowToContents(row);
  }
}

cedar::proc::gui::PropertyPane::DataWidgetTypes& cedar::proc::gui::PropertyPane::dataWidgetTypes()
{
  if (cedar::proc::gui::PropertyPane::mDataWidgetTypes.empty())
  {
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::DoubleParameter, cedar::proc::gui::DoubleParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::UIntParameter, cedar::proc::gui::UIntParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::StringParameter, cedar::proc::gui::StringParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::BoolParameter, cedar::proc::gui::BoolParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::DoubleVectorParameter, cedar::proc::gui::DoubleVectorParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::UIntVectorParameter, cedar::proc::gui::UIntVectorParameter>();
  }
  return cedar::proc::gui::PropertyPane::mDataWidgetTypes;
}

void cedar::proc::gui::PropertyPane::redraw()
{
  if (mDisplayedStep)
  {
    this->display(mDisplayedStep);
  }
}
