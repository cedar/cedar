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
  int button_size = 20;

  QGridLayout *p_layout = new QGridLayout();
  p_layout->setContentsMargins(margins, margins, margins, margins);
  p_layout->setSpacing(spacing);
  this->setLayout(p_layout);

  mpTypeSelector = new QComboBox();
  p_layout->addWidget(mpTypeSelector, 0, 0);

  mpEditButton = new QPushButton("...");
  mpEditButton->setMaximumWidth(button_size);
  mpEditButton->setEnabled(false);
  p_layout->addWidget(mpEditButton, 0, 1);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
  QObject::connect(this->mpEditButton, SIGNAL(clicked()), this, SLOT(editClicked()));
}

cedar::aux::gui::ObjectParameter::~ObjectParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
    this->mpTypeSelector->addItem(QString::fromStdString(types.at(i)));
    if (current_type == -1 && types.at(i) == parameter->getTypeId())
    {
      current_type = static_cast<int>(i);
    }
  }

  this->mpTypeSelector->setCurrentIndex(current_type);
  this->mpEditButton->setEnabled(current_type != -1);

  // reconnect the signal
  QObject::connect(this->mpTypeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentTypeChanged(int)));

}

void cedar::aux::gui::ObjectParameter::editClicked()
{
  //!@todo Store the pointers to the dialogs/property panes hare and update them when an item is added/removed/...
  //!@todo Find a way to make sure these dialogs close when the main window does.
  cedar::aux::ConfigurablePtr configurable = this->getObjectParameter()->getConfigurable();
  cedar::aux::gui::PropertyPane *p_display = new cedar::aux::gui::PropertyPane();
  QDialog *p_dialog = new QDialog();
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_dialog->setLayout(p_layout);
  p_layout->addWidget(p_display);

  p_dialog->setWindowTitle(QString::fromStdString(this->getObjectParameter()->getName()));

  p_dialog->show();
  p_display->display(configurable);

  // resize to fit contents
  p_display->adjustSize();
  p_dialog->adjustSize();
}

std::string cedar::aux::gui::ObjectParameter::getSelectedType() const
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

void cedar::aux::gui::ObjectParameter::currentTypeChanged(int index)
{
  this->mpEditButton->setEnabled(index != -1);

  if (index != -1)
  {
    std::string type = this->mpTypeSelector->currentText().toStdString();
    this->getObjectParameter()->setType(type);
  }
}
