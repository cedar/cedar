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
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/auxiliaries/ObjectListParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <boost/bind.hpp>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
QTableWidget(pParent)
{
  this->setSelectionMode(QAbstractItemView::NoSelection);
  this->setTabKeyNavigation(false);
  this->setAlternatingRowColors(true);
  this->horizontalHeader()->setStretchLastSection(true);
  this->horizontalHeader()->setVisible(true);
  this->verticalHeader()->setVisible(false);

  this->setColumnCount(2);

  QStringList labels;
  labels << "Property" << "Value";
  this->setHorizontalHeaderLabels(labels);
}

cedar::aux::gui::PropertyPane::~PropertyPane()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PropertyPane::resetContents()
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
  this->resetPointer();
  this->mParameterWidgetRowIndex.clear();
  this->mParameterRowIndex.clear();
}

void cedar::aux::gui::PropertyPane::disconnect(cedar::aux::ConfigurablePtr pConfigurable)
{
  for (cedar::aux::Configurable::ParameterList::iterator iter = pConfigurable->getParameters().begin();
      iter != pConfigurable->getParameters().end();
      ++iter)
  {
    // disconnect everything between the parameter and this
    if (!QObject::disconnect(iter->get(), 0, this, 0))
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not disconnect the slots of the Property pane.",
        "cedar::proc::gui::PropertyPane::disconnect(cedar::aux::ConfigurablePtr)"
      );
    }
  }

  for (cedar::aux::Configurable::Children::const_iterator iter = pConfigurable->configurableChildren().begin();
       iter != pConfigurable->configurableChildren().end();
       ++iter)
  {
    this->disconnect(iter->second);
  }
}

std::string cedar::aux::gui::PropertyPane::getInstanceTypeId(cedar::aux::ConfigurablePtr pConfigurable) const
{
  return cedar::aux::objectTypeToString(pConfigurable);
}

void cedar::aux::gui::PropertyPane::display(cedar::aux::ConfigurablePtr pConfigurable)
{
  this->resetContents();

  this->mDisplayedConfigurable = pConfigurable;

  std::string label = this->getInstanceTypeId(pConfigurable);
  this->addLabelRow(label);

  this->append(pConfigurable->getParameters());
  mSlotConnection
    = pConfigurable->connectToTreeChangedSignal(boost::bind(&cedar::aux::gui::PropertyPane::redraw, this));
  for (cedar::aux::Configurable::Children::const_iterator iter = pConfigurable->configurableChildren().begin();
       iter != pConfigurable->configurableChildren().end();
       ++iter)
  {
    this->addHeadingRow(iter->first);
    this->append(iter->second->getParameters());
  }
}

void cedar::aux::gui::PropertyPane::addHeadingRow(const std::string& label)
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

void cedar::aux::gui::PropertyPane::addLabelRow(const std::string& label)
{
  int row = this->rowCount();
  this->insertRow(row);
  QLabel *p_label = new QLabel();
  p_label->setText(label.c_str());
  this->setCellWidget(row, 0, p_label);
  this->setSpan(row, 0, 1, 2);
}

void cedar::aux::gui::PropertyPane::append(cedar::aux::Configurable::ParameterList& parameters)
{
  for (cedar::aux::Configurable::ParameterList::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
  {
    this->addPropertyRow(*iter);
  }
}

void cedar::aux::gui::PropertyPane::indicateChange(QLabel *pLabel, bool changed)
{
  QFont font = pLabel->font();
  font.setBold(changed);
  pLabel->setFont(font);
}

void cedar::aux::gui::PropertyPane::addPropertyRow(cedar::aux::ParameterPtr parameter)
{
  if (!parameter->isHidden())
  {
    // check if parameter is an object parameter
    if
    (
      boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(parameter)
      || boost::dynamic_pointer_cast<cedar::aux::ObjectListParameter>(parameter)
    )
    {
      this->addHeadingRow(parameter->getName());
    }

    int row = this->rowCount();
    this->insertRow(row);
    QLabel *p_label = new QLabel();
    p_label->setText(parameter->getName().c_str());
    this->setCellWidget(row, 0, p_label);

    this->indicateChange(p_label, parameter->isChanged());


    cedar::aux::gui::Parameter *p_widget
      = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    p_widget->setParent(this);
    p_widget->setParameter(parameter);
    p_widget->setEnabled(!parameter->isConstant());
    this->setCellWidget(row, 1, p_widget);
    this->resizeRowToContents(row);

    this->mParameterWidgetRowIndex[p_widget] = row;
    this->mParameterRowIndex[parameter.get()] = row;
    QObject::connect(p_widget, SIGNAL(heightChanged()), this, SLOT(rowSizeChanged()));
    QObject::connect(parameter.get(), SIGNAL(changedFlagChanged()), this, SLOT(parameterChangeFlagChanged()));

    // check if parameter is an object parameter
    if
    (
      cedar::aux::ObjectParameterPtr object_parameter
        = boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(parameter)
    )
    {
      cedar::aux::ConfigurablePtr configurable = object_parameter->getConfigurable();
      this->append(configurable->getParameters());
      QObject::connect(object_parameter.get(), SIGNAL(valueChanged()), this, SLOT(redraw()));
    }

    // check if parameter is an object list parameter
    if
    (
      cedar::aux::ObjectListParameterPtr list_parameter
        = boost::dynamic_pointer_cast<cedar::aux::ObjectListParameter>(parameter)
    )
    {
      QObject::connect(list_parameter.get(), SIGNAL(valueChanged()), this, SLOT(redraw()));

      for (size_t i = 0; i < list_parameter->size(); ++i)
      {
        cedar::aux::ConfigurablePtr configurable = list_parameter->configurableAt(i);
        std::string label = list_parameter->getName() + "[" + cedar::aux::toString(i) + "]";
        this->addLabelRow(label);
        this->append(configurable->getParameters());
      }
    }
  }
}

int cedar::aux::gui::PropertyPane::getSenderParameterRowWidget() const
{
  cedar::aux::gui::Parameter *p_parameter = dynamic_cast<cedar::aux::gui::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  CEDAR_DEBUG_ASSERT(this->mParameterWidgetRowIndex.find(p_parameter) != this->mParameterWidgetRowIndex.end());
  return this->mParameterWidgetRowIndex.find(p_parameter)->second;
}

int cedar::aux::gui::PropertyPane::getSenderParameterRow() const
{
  cedar::aux::Parameter *p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  CEDAR_DEBUG_ASSERT(this->mParameterRowIndex.find(p_parameter) != this->mParameterRowIndex.end());
  return this->mParameterRowIndex.find(p_parameter)->second;
}

void cedar::aux::gui::PropertyPane::parameterChangeFlagChanged()
{
  cedar::aux::Parameter *p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  int row = this->getSenderParameterRow();

  QLabel *p_label = dynamic_cast<QLabel*>(this->cellWidget(row, 0)); // column 0 contains the label
  CEDAR_DEBUG_ASSERT(p_label != NULL);

  this->indicateChange(p_label, p_parameter->isChanged());
}

void cedar::aux::gui::PropertyPane::rowSizeChanged()
{
  int row = this->getSenderParameterRowWidget();

  // the process-events call is only necessary because qt does otherwise not detect the new size properly.
  // should this bug ever be fixed, this can be removed.
  QApplication::processEvents();

  this->resizeRowToContents(row);
}

void cedar::aux::gui::PropertyPane::resetPointer()
{
  this->mDisplayedConfigurable.reset();
  this->clearContents();
  this->setRowCount(0);
}

void cedar::aux::gui::PropertyPane::redraw()
{
  int current_scroll_position = 0;
  if (QScrollBar *p_scroll_bar = this->verticalScrollBar())
  {
    current_scroll_position = p_scroll_bar->value();
  }

  cedar::aux::ConfigurablePtr displayed = this->mDisplayedConfigurable.lock();
  this->resetContents();
  this->display(displayed);

  if (QScrollBar *p_scroll_bar = this->verticalScrollBar())
  {
    p_scroll_bar->setValue(current_scroll_position);
  }
}
