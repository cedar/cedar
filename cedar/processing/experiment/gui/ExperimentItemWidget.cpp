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

    File:        ExperimentItemWidget.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 19

    Description: Source file for the class cedar::proc::experiment::gui::ExperimentItemWidget.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ExperimentItemWidget.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"
// SYSTEM INCLUDES
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ExperimentItemWidget::ExperimentItemWidget(QWidget* parent)
:
QWidget(parent)
{
  this->setLayout(new QHBoxLayout);
  this->layout()->setMargin(0);
}

cedar::proc::experiment::gui::ExperimentItemWidget::~ExperimentItemWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::ExperimentItemWidget::display(cedar::aux::ConfigurablePtr experimentItem)
{
  clear();
  for (auto parameter : experimentItem->getParameters())
  {
    cedar::aux::gui::Parameter* parameterWidget;
    parameterWidget = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    parameterWidget->setParameter(parameter);
    parameterWidget->setParent(this);

    if (parameter->hasSingleConfigurableChild())
    {
      QVBoxLayout* row = new QVBoxLayout;
      row->setAlignment(Qt::AlignTop);
      cedar::proc::experiment::gui::ExperimentItemWidget* another_widget = new cedar::proc::experiment::gui::ExperimentItemWidget();
      another_widget->display(parameter->getSingleConfigurableChild());
      row->addWidget(parameterWidget);
      row->addWidget(another_widget);
      static_cast<QBoxLayout*>(this->layout())->addLayout(row);
      connect(parameter.get(),SIGNAL(valueChanged()),this,SLOT(objectParameterChanged()));
    }
    else
    {
      QVBoxLayout* p_column = new QVBoxLayout();
      p_column->setAlignment(Qt::AlignCenter);
      QLabel* p_label = new QLabel();
      p_label->setText(QString::fromStdString(parameter->getName()));
      p_column->addWidget(p_label);
      p_column->addWidget(parameterWidget);
      static_cast<QBoxLayout*>(this->layout())->addLayout(p_column);
    }
  }
}


void cedar::proc::experiment::gui::ExperimentItemWidget::clear(QLayout* layout)
{
  if (layout == NULL)
  {
    clear(this->layout());
    return;
  }
  QLayoutItem* child;
  while((child = layout->takeAt(0)) != 0)
  {
      if(child->layout() != 0)
      {
        //clear(child->layout());
      }
      else if(child->widget() != 0)
      {
        delete child->widget();
      }
      delete child;
  }
}


void cedar::proc::experiment::gui::ExperimentItemWidget::objectParameterChanged()
{
  auto p_parameter = dynamic_cast<cedar::aux::Parameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_parameter != nullptr);
  CEDAR_DEBUG_ASSERT(p_parameter->hasSingleConfigurableChild());

  QList<cedar::aux::gui::Parameter*> parameter_list = this->findChildren<cedar::aux::gui::Parameter*>();
  for(cedar::aux::gui::Parameter* parameter : parameter_list)
  {
    if(parameter->getParameter().get() == p_parameter)
    {
      QLayoutItem* child;
      for(int i=0; i < this->layout()->count() ; i++)
      {
        child = layout()->itemAt(i);
        if(child->layout())
        {
          QWidget* widget = child->layout()->itemAt(0)->widget();
          if (widget == parameter)
          {
            delete child->layout()->itemAt(1)->widget();
            cedar::proc::experiment::gui::ExperimentItemWidget* widget
                    = new cedar::proc::experiment::gui::ExperimentItemWidget();
            widget->display(p_parameter->getSingleConfigurableChild());
            child->layout()->addWidget(widget);
            return;

          }
        }
      }
    }
  }
}

