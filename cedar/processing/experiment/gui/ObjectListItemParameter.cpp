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

    File:        ObjectListItemParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 06 17

    Description: Source file for the class cedar::proc::experiment::gui::ObjectListItemParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ObjectListItemParameter.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ObjectListItemParameter::ObjectListItemParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
,
mpObjectSelector(new cedar::aux::gui::ObjectParameter())
,
mpObjectItem(new cedar::proc::experiment::gui::ExperimentItemWidget())
{
  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget(mpObjectSelector);
  layout->addWidget(mpObjectItem);
  this->setLayout(layout);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::ObjectListItemParameter::~ObjectListItemParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::experiment::gui::ObjectListItemParameter::parameterPointerChanged()
{
  mpObjectSelector->setParameter(this->getParameter());
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(this->getParameter());
  mpObjectItem->display(parameter->getConfigurable());
  connect(parameter.get(),SIGNAL(valueChanged()),this,SLOT(valueChanged()));
  this->valueChanged();
  std::cout << "parameterPonterChanged" << std::endl;
}

void cedar::proc::experiment::gui::ObjectListItemParameter::valueChanged()
{
  mpObjectSelector->setParameter(this->getParameter());
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::ObjectParameter>(this->getParameter());
  mpObjectItem->display(parameter->getConfigurable());
  std::cout << "valueChanged" << std::endl;
  std::cout << parameter->getConfigurable() << std::endl;
}
