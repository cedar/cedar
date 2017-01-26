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

    File:        ProjectionMappingParameter.cpp
    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de

    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/ProjectionMappingParameter.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QColor>
#include <QPalette>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::proc::ProjectionMappingParameter,
        cedar::proc::gui::ProjectionMappingParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ProjectionMappingParameter::ProjectionMappingParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QVBoxLayout());
  this->layout()->setContentsMargins(0, 0, 0, 0);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
  updateValidity();
}

//!@brief Destructor
cedar::proc::gui::ProjectionMappingParameter::~ProjectionMappingParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ProjectionMappingParameter::parameterPointerChanged()
{
  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(valueChanged()));
  QObject::connect(parameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertyChanged()));
  this->propertyChanged();
}

void cedar::proc::gui::ProjectionMappingParameter::valueChanged()
{
  updateValidity();
}

void cedar::proc::gui::ProjectionMappingParameter::updateValidity()
{
  QColor color;

  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  for
  (
    std::vector<QComboBox*>::iterator iter = mComboBoxes.begin();
    iter != mComboBoxes.end();
    ++iter
  )
  {
    if (parameter->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_ERROR)
    {
      std::string style_red =
      "QComboBox {\
           border: 1px solid gray;\
           border-radius: 3px;\
           padding: 1px 18px 1px 3px;\
           min-width: 6em;\
       }\
       QComboBox:editable {\
           background: white;\
       }\
       QComboBox:!editable, QComboBox::drop-down:editable {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                        stop: 0 #ff0000, stop: 0.4 #ee1111,\
                                        stop: 0.5 #dd1111, stop: 1.0 #bb3333);\
       }\
      QComboBox::drop-down {\
           subcontrol-origin: padding;\
           subcontrol-position: top right;\
           width: 15px;\
           border-left-width: 0px;\
           border-left-color: darkgray;\
           border-left-style: solid;\
           border-top-right-radius: 3px;\
           border-bottom-right-radius: 3px;\
       }";

      (*iter)->setStyleSheet(style_red.c_str());
    }
    else if (parameter->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_WARNING)
    {
      std::string style_yellow =
      "QComboBox {\
           border: 1px solid gray;\
           border-radius: 3px;\
           padding: 1px 18px 1px 3px;\
           min-width: 6em;\
       }\
       QComboBox:editable {\
           background: white;\
       }\
       QComboBox:!editable, QComboBox::drop-down:editable {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                  stop: 0 #ffcc00, stop: 0.4 #fac800,\
                                  stop: 0.5 #e6b800, stop: 1.0 #e6b800);\
       }\
      QComboBox::drop-down {\
           subcontrol-origin: padding;\
           subcontrol-position: top right;\
           width: 15px;\
           border-left-width: 0px;\
           border-left-color: darkgray;\
           border-left-style: solid;\
           border-top-right-radius: 3px;\
           border-bottom-right-radius: 3px;\
       }";

      (*iter)->setStyleSheet(style_yellow.c_str());
    }
    else if (parameter->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_VALID)
    {
      std::string style_green =
      "QComboBox {\
           border: 1px solid gray;\
           border-radius: 3px;\
           padding: 1px 18px 1px 3px;\
           min-width: 6em;\
       }\
       QComboBox:editable {\
           background: white;\
       }\
       QComboBox:!editable, QComboBox::drop-down:editable {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                        stop: 0 #00ff00, stop: 0.4 #11ee11,\
                                        stop: 0.5 #11dd11, stop: 1.0 #33bb33);\
       }\
      QComboBox::drop-down {\
           subcontrol-origin: padding;\
           subcontrol-position: top right;\
           width: 15px;\
           border-left-width: 0px;\
           border-left-color: darkgray;\
           border-left-style: solid;\
           border-top-right-radius: 3px;\
           border-bottom-right-radius: 3px;\
       }";

      (*iter)->setStyleSheet(style_green.c_str());
    }
  }
}

void cedar::proc::gui::ProjectionMappingParameter::propertyChanged()
{
  // get the parameter
  cedar::proc::ProjectionMappingParameterPtr parameter
    = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  // delete all the combo boxes
  for (size_t i = 0; i < this->mComboBoxes.size(); ++i)
  {
    delete this->mComboBoxes.at(i);
  }
  this->mComboBoxes.clear();

  if (parameter->getValue()->getOutputDimensionality() > 0)
  {
    // create a list of options to which output dimensions each input can be mapped,
    // including the option to drop an input
    QStringList output_dimensions;
    output_dimensions.push_back("drop");
    for (unsigned int i = 0; i < parameter->getValue()->getOutputDimensionality(); ++i)
    {
      output_dimensions.push_back(cedar::aux::toString<unsigned int>(i).c_str());
    }

    for (size_t i = 0; i < parameter->getValue()->getNumberOfMappings(); ++i)
    {
      QComboBox *p_widget = new QComboBox();
      this->mComboBoxes.push_back(p_widget);
      this->layout()->addWidget(p_widget);
      p_widget->setMinimumHeight(20);

      this->mComboBoxes.at(i)->addItems(output_dimensions);

      unsigned int current_index;

      if (parameter->getValue()->isDropped(i))
      {
        current_index = 0;
      }
      else
      {
        current_index = parameter->getValue()->lookUp(i) + 1;
      }

      p_widget->setCurrentIndex(current_index);
      p_widget->setDisabled(parameter->isConstant());

      QObject::connect(p_widget, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

      emit heightChanged();
    }

    updateValidity();
  }
}

void cedar::proc::gui::ProjectionMappingParameter::currentIndexChanged(int)
{
  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  for (size_t i = 0; i < this->mComboBoxes.size(); ++i)
  {
    if (this->mComboBoxes.at(i)->currentIndex() == 0)
    {
      parameter->drop(i);
    }
    else
    {
      unsigned int output_dimension = static_cast<unsigned int>(this->mComboBoxes.at(i)->currentIndex() - 1);
      parameter->changeMapping(i, output_dimension);
    }
  }
}
