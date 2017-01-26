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

    File:        ObjectParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ObjectParameter.h"
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QDialog>
#include <QEvent>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->addDerived
      <
        cedar::aux::ObjectParameter,
        cedar::aux::gui::ObjectParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ObjectParameter::ObjectParameter()
{
  int margins = 0;
  int spacing = 2;

  QGridLayout *p_layout = new QGridLayout();
  p_layout->setContentsMargins(margins, margins, margins, margins);
  p_layout->setSpacing(spacing);
  this->setLayout(p_layout);

  // create the widget used for selecting a type
  this->mpTypeSelector = new QComboBox();
  this->mpTypeSelector->setToolTip("Select the type of the object.");

  // this sets up the event filter that prevents changes via mouse-wheel scrolling
  // this was a problem because these widgets were often accidentally be changed while scrolling
  this->mpTypeSelector->setFocusPolicy(Qt::StrongFocus);
  this->mpTypeSelector->installEventFilter(this);

  // add the widget
  p_layout->addWidget(mpTypeSelector, 0, 0);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::aux::gui::ObjectParameter::~ObjectParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::gui::ObjectParameter::eventFilter(QObject* object, QEvent* event)
{
  // check that the event is really a wheel event and refers to a combo box
  if
  (
    event->type() == QEvent::Wheel &&
    qobject_cast<QComboBox*>(object)
  )
  {
    // if so, ignore the event
    event->ignore();
    return true;
  }
  // otherwise,
  return QWidget::eventFilter(object, event);
}

void cedar::aux::gui::ObjectParameter::splitTypeId(const QString& typeId, QString& className) const
{
  QStringList parts = typeId.split('.');
  className = parts.back();
//  parts.pop_back();
//  namespaceName = parts.join(".");
}

void cedar::aux::gui::ObjectParameter::parameterPointerChanged()
{
  cedar::aux::ObjectParameterPtr parameter = this->getObjectParameter();

  // Fill types -------------------------------------------------------------------------------

  // disconnect the signal while we fill the box
  QObject::disconnect(this->mpTypeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentTypeChanged(int)));

  this->mpTypeSelector->clear();

  std::vector<std::string> types;
  parameter->listTypes(types);

  int current_type = -1;
  for (size_t i = 0; i < types.size(); ++i)
  {
    QString type_id = QString::fromStdString(types.at(i));
    QString class_name;
    this->splitTypeId(type_id, class_name);
    this->mpTypeSelector->addItem(class_name);
    this->mpTypeSelector->setItemData(i, type_id);
    this->mpTypeSelector->setItemData(i, type_id, Qt::ToolTipRole);
    if (current_type == -1 && types.at(i) == parameter->getTypeId())
    {
      current_type = static_cast<int>(i);
    }
  }

  this->mpTypeSelector->setCurrentIndex(current_type);

  // reconnect the signal
  QObject::connect(this->mpTypeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentTypeChanged(int)));
}

std::string cedar::aux::gui::ObjectParameter::getSelectedType() const
{
  int index = this->mpTypeSelector->currentIndex();
  if (index != -1)
  {
    return this->mpTypeSelector->itemData(index).toString().toStdString();
  }
  else
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "No type selected.");
  }
}

void cedar::aux::gui::ObjectParameter::currentTypeChanged(int index)
{
  if (index != -1)
  {
    std::string type = this->getSelectedType();
    this->getObjectParameter()->setType(type);
  }
}
