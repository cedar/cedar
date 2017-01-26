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
#include "cedar/auxiliaries/Settings.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#ifndef Q_MOC_RUN
  #include <boost/bind.hpp>
#endif


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
QTableWidget(pParent),
mpShowAdvanced(NULL)
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

  // advanced button
  mpShowAdvanced = new QCheckBox("show advanced");
  QObject::connect(mpShowAdvanced, SIGNAL(toggled(bool)), this, SLOT(showAdvanced(bool)));
  this->addWidgetRow(this->mpShowAdvanced);
}

cedar::aux::gui::PropertyPane::~PropertyPane()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PropertyPane::showAdvanced(bool show)
{
  if (this->mpShowAdvanced != NULL)
  {
    this->mpShowAdvanced->setChecked(show);
  }
  this->redraw();
}

void cedar::aux::gui::PropertyPane::resetContents()
{
  this->mDisplayedConfigurable.reset();
  mpShowAdvanced->setText("show advanced");
  this->setRowCount(1);

  this->disconnect();

  // disconnect all signals from the configurable
  this->mParameterWidgetRowIndex.clear();
  this->mParameterRowIndex.clear();
}

void cedar::aux::gui::PropertyPane::disconnect()
{
  for (size_t i = 0; i < this->mSlotConnections.size(); i++)
  {
    if (this->mSlotConnections[i].connected())
    {
      this->mSlotConnections[i].disconnect();
    }
  }
  this->mSlotConnections.clear();

  for (auto iter = this->mParameterRowIndex.begin(); iter != this->mParameterRowIndex.end(); ++iter)
  {
    cedar::aux::Parameter* parameter = iter->first;

    // disconnect everything between the parameter and this
    if (!QObject::disconnect(parameter, 0, this, 0))
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Could not disconnect the slots from parameter \"" + parameter->getName() + "\" to the property pane.",
        "cedar::proc::gui::PropertyPane::disconnect(cedar::aux::ConfigurablePtr)"
      );
    }
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
#ifdef DEBUG
  if (cedar::aux::SettingsSingleton::getInstance()->getMemoryDebugOutput())
  {
    label += " " + cedar::aux::toString(pConfigurable.get());
  }
#endif // DEBUG
  this->addLabelRow(label);

  QString adv_text = QString("show advanced (%1)").arg(pConfigurable->countAdvanced());
  mpShowAdvanced->setText(adv_text);

  this->append("", pConfigurable);
}

void cedar::aux::gui::PropertyPane::append(const std::string& title, cedar::aux::ConfigurablePtr configurable)
{
  if (!title.empty())
  {
    this->addHeadingRow(title);
  }

  // append the direct parameters of the configurable
  this->append(configurable->getParameters());

  mSlotConnections.push_back
  (
    configurable->connectToTreeChangedSignal(boost::bind(&cedar::aux::gui::PropertyPane::redraw, this))
  );

  // append all children as well
  for (cedar::aux::Configurable::Children::const_iterator iter = configurable->configurableChildren().begin();
       iter != configurable->configurableChildren().end();
       ++iter)
  {
    const std::string& label = iter->first;
    cedar::aux::ConfigurablePtr configurable = iter->second;

    if (!configurable->isAdvanced() || this->showAdvanced())
    {
      this->append(label, configurable);
    }
  }
}

void cedar::aux::gui::PropertyPane::addHeadingRow(const std::string& label)
{
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

  this->addWidgetRow(p_label);
}

void cedar::aux::gui::PropertyPane::addLabelRow(const std::string& label)
{
  QLabel *p_label = new QLabel();
  p_label->setText(label.c_str());
  this->addWidgetRow(p_label);
}

void cedar::aux::gui::PropertyPane::addWidgetRow(QWidget* pWidget)
{
  int row = this->rowCount();
  this->insertRow(row);
  this->setCellWidget(row, 0, pWidget);
  this->setSpan(row, 0, 1, 2);
}

void cedar::aux::gui::PropertyPane::append(cedar::aux::Configurable::ParameterList& parameters)
{
  for (cedar::aux::Configurable::ParameterList::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
  {
    cedar::aux::ParameterPtr parameter = *iter;
    if (!parameter->isAdvanced() || this->showAdvanced())
    {
      this->addPropertyRow(parameter);
    }
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


    try
    {
      cedar::aux::gui::Parameter *p_widget
        = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
      p_widget->setParent(this);
      p_widget->setParameter(parameter);
      this->setCellWidget(row, 1, p_widget);
      this->resizeRowToContents(row);

      this->mParameterWidgetRowIndex[p_widget] = row;
      this->mParameterRowIndex[parameter.get()] = row;
      QObject::connect(p_widget, SIGNAL(heightChanged()), this, SLOT(rowSizeChanged()));
      QObject::connect(parameter.get(), SIGNAL(changedFlagChanged()), this, SLOT(parameterChangeFlagChanged()));
    }
    catch (cedar::aux::UnknownTypeException& e)
    {
      QLabel* p_no_widget_label = new QLabel("(No widget for type)");
      p_no_widget_label->setToolTip(QString::fromStdString(e.exceptionInfo()));
      p_no_widget_label->setAlignment(Qt::AlignCenter);
      p_no_widget_label->setAutoFillBackground(true);
      p_no_widget_label->setStyleSheet("background-color: rgb(255, 200, 137)");
      this->setCellWidget(row, 1, p_no_widget_label);
      this->resizeRowToContents(row);
    }

    // check if parameter is an object parameter
    if
    (
      cedar::aux::ObjectParameterPtr object_parameter
        = boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(parameter)
    )
    {
      cedar::aux::ConfigurablePtr configurable = object_parameter->getSingleConfigurableChild();
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
        cedar::aux::ConfigurablePtr configurable = list_parameter->getConfigurableChild(i);
        std::string label = list_parameter->getName() + "[" + cedar::aux::toString(i) + "]";
        this->append(label, configurable);
      }
    }
  }
}

int cedar::aux::gui::PropertyPane::getSenderParameterRowWidget() const
{
  cedar::aux::gui::Parameter *p_parameter = dynamic_cast<cedar::aux::gui::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != NULL);

  if (this->mParameterWidgetRowIndex.find(p_parameter) == this->mParameterWidgetRowIndex.end())
  {
    CEDAR_THROW(cedar::aux::ParameterNotFoundException, "cannot find given parameter's widget");
  }
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
  int row;
  try
  {
    row = this->getSenderParameterRowWidget();
  }
  catch (cedar::aux::ParameterNotFoundException& exc)
  {
    return;
  }

  // the process-events call is only necessary because qt does otherwise not detect the new size properly.
  // should this bug ever be fixed, this can be removed.
  // in addition, we have to exclude user input events, otherwise, they interfere with previous unprocessed events
  // (which might lead to deadlocks)
  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

  this->resizeRowToContents(row);
}

void cedar::aux::gui::PropertyPane::redraw()
{
  int current_scroll_position = 0;
  if (QScrollBar *p_scroll_bar = this->verticalScrollBar())
  {
    current_scroll_position = p_scroll_bar->value();
  }

  cedar::aux::ConfigurablePtr displayed = this->mDisplayedConfigurable.lock();
  if (displayed)
  {
    this->resetContents();
    this->display(displayed);
  }

  if (QScrollBar *p_scroll_bar = this->verticalScrollBar())
  {
    p_scroll_bar->setValue(current_scroll_position);
  }
}
