/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
  for(cedar::aux::ParameterPtr parameter : experimentItem->getParameters())
  {
    cedar::aux::gui::Parameter* parameterWidget;
    parameterWidget = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    parameterWidget->setParameter(parameter);
    parameterWidget->setParent(this);

    if(auto object_parameter = boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(parameter))
    {
      QVBoxLayout* row = new QVBoxLayout;
      row->setAlignment(Qt::AlignTop);
      cedar::proc::experiment::gui::ExperimentItemWidget* another_widget = new cedar::proc::experiment::gui::ExperimentItemWidget();
      another_widget->display(object_parameter->getConfigurable());
      row->addWidget(parameterWidget);
      row->addWidget(another_widget);
      ((QBoxLayout*)this->layout())->addLayout(row);
      connect(object_parameter.get(),SIGNAL(valueChanged()),this,SLOT(objectParameterChanged()));
    }
    else
    {
      this->layout()->addWidget(parameterWidget);
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
  auto p_objec_parameter = dynamic_cast<cedar::aux::ObjectParameter*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_objec_parameter != nullptr);


  QList<cedar::aux::gui::Parameter*> parameter_list = this->findChildren<cedar::aux::gui::Parameter*>();
  for(cedar::aux::gui::Parameter* parameter : parameter_list)
  {
    if(parameter->getParameter().get() == p_objec_parameter)
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
            widget->display(p_objec_parameter->getConfigurable());
            child->layout()->addWidget(widget);
            return;

          }
        }
      }
    }
  }
}

