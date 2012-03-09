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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ObjectListParameter.h"
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QDialog>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->addDerived
      <
        cedar::aux::ObjectListParameter,
        cedar::aux::gui::ObjectListParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ObjectListParameter::ObjectListParameter()
{
  int margins = 0;
  int spacing = 2;
  int button_size = 20;

  QGridLayout *p_layout = new QGridLayout();
  p_layout->setContentsMargins(margins, margins, margins, margins);
  p_layout->setSpacing(spacing);
  this->setLayout(p_layout);

  mpTypeSelector = new QComboBox();
  p_layout->addWidget(mpTypeSelector, 0, 0);

  mpAddButton = new QPushButton("+");
  mpAddButton->setMaximumWidth(button_size);
  p_layout->addWidget(mpAddButton, 0, 1);

  mpInstanceSelector = new QComboBox();
  p_layout->addWidget(mpInstanceSelector, 1, 0);

  mpRemoveButton = new QPushButton("-");
  mpRemoveButton->setMaximumWidth(button_size);
  mpRemoveButton->setEnabled(false);
  p_layout->addWidget(mpRemoveButton, 1, 1);

  mpEditButton = new QPushButton("...");
  mpEditButton->setMaximumWidth(button_size);
  mpEditButton->setEnabled(false);
  p_layout->addWidget(mpEditButton, 1, 2);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  QObject::connect(this->mpRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
  QObject::connect(this->mpEditButton, SIGNAL(clicked()), this, SLOT(editClicked()));
  QObject::connect(this->mpInstanceSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentInstanceIndexChanged(int)));
}

cedar::aux::gui::ObjectListParameter::~ObjectListParameter()
{
  mObjectAddedConnection.disconnect();
  mObjectRemovedConnection.disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ObjectListParameter::parameterPointerChanged()
{
  cedar::aux::ObjectListParameterPtr parameter = this->getObjectList();

  // Disconnect old connections (if any) ------------------------------------------------------
  this->mObjectAddedConnection.disconnect();
  this->mObjectRemovedConnection.disconnect();

  // Connect to the new parameter's signals ---------------------------------------------------
  this->mObjectAddedConnection = parameter->connectToObjectAddedSignal
                                 (
                                   boost::bind
                                   (
                                     &cedar::aux::gui::ObjectListParameter::slotObjectAdded,
                                     this,
                                     _1
                                   )
                                 );

  this->mObjectRemovedConnection = parameter->connectToObjectRemovedSignal
                                   (
                                     boost::bind
                                     (
                                       &cedar::aux::gui::ObjectListParameter::slotObjectRemoved,
                                       this,
                                       _1
                                     )
                                   );

  // Fill types -------------------------------------------------------------------------------
  this->mpTypeSelector->clear();

  std::vector<std::string> types;
  parameter->listTypes(types);

  for (size_t i = 0; i < types.size(); ++i)
  {
    this->mpTypeSelector->addItem(QString::fromStdString(types.at(i)));
  }

  // Fill existing instances ------------------------------------------------------------------
  this->mpInstanceSelector->clear();
  for (size_t i = 0; i < parameter->size(); ++i)
  {
    this->appendObjectToInstanceList(i);
  }
}

void cedar::aux::gui::ObjectListParameter::addClicked()
{
  this->getObjectList()->pushBack(this->getSelectedType());
}

void cedar::aux::gui::ObjectListParameter::slotObjectAdded(int index)
{
  this->appendObjectToInstanceList(index);
}

void cedar::aux::gui::ObjectListParameter::appendObjectToInstanceList(int index)
{
  cedar::aux::ConfigurablePtr object = this->getObjectList()->configurableAt(index);
  const std::string& instance_type = this->getObjectList()->getTypeOfObject(object);
  QString label = QString::fromStdString(instance_type);
  this->mpInstanceSelector->addItem(label);

  // The object's index should always correspond to the index in the combo box.
  CEDAR_DEBUG_ASSERT(index == this->mpInstanceSelector->count() - 1);
}

void cedar::aux::gui::ObjectListParameter::removeClicked()
{
  int index = this->mpInstanceSelector->currentIndex();
  if (index != -1)
  {
    this->getObjectList()->removeObject(index);
  }
  else
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "No instance selected.");
  }
}

void cedar::aux::gui::ObjectListParameter::slotObjectRemoved(int index)
{
  this->mpInstanceSelector->removeItem(index);
}

void cedar::aux::gui::ObjectListParameter::editClicked()
{
  //!@todo Store the pointers to the dialogs/property panes hare and update them when an item is added/removed/...
  //!@todo Find a way to make sure these dialogs close when the main window does.
  cedar::aux::ConfigurablePtr configurable = this->getSelectedInstance();
  cedar::aux::gui::PropertyPane *p_display = new cedar::aux::gui::PropertyPane();
  QDialog *p_dialog = new QDialog();
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_dialog->setLayout(p_layout);
  p_layout->addWidget(p_display);

  QString title
    = QString::fromStdString(this->getObjectList()->getName())
      + QString("[%1]").arg(this->mpInstanceSelector->currentIndex());

  p_dialog->setWindowTitle(title);

  p_dialog->show();
  p_display->display(configurable);

  // resize to fit contents
  p_display->adjustSize();
  p_dialog->adjustSize();
}

std::string cedar::aux::gui::ObjectListParameter::getSelectedType() const
{
  int index = this->mpTypeSelector->currentIndex();
  if (index != -1)
  {
    return this->mpTypeSelector->currentText().toStdString();
  }
  else
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "No type selected.");
  }
}

cedar::aux::ConfigurablePtr cedar::aux::gui::ObjectListParameter::getSelectedInstance()
{
  int index = this->mpInstanceSelector->currentIndex();
  if (index != -1)
  {
    cedar::aux::ObjectListParameterPtr parameter = this->getObjectList();
    return parameter->configurableAt(static_cast<size_t>(index));
  }
  else
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "No instance selected.");
  }
}

void cedar::aux::gui::ObjectListParameter::currentInstanceIndexChanged(int index)
{
  bool enabled = (index != -1);
  mpRemoveButton->setEnabled(enabled);
  mpEditButton->setEnabled(enabled);
}
