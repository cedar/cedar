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

    File:        ToolBox.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/ToolBox.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVariant>
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ToolBox::ToolBox(unsigned int columns, QWidget *pParent)
:
QWidget(pParent),
mColumns(columns),
mpSelectedButton(NULL)
{
  QVBoxLayout *p_outer_layout = new QVBoxLayout();
  this->setLayout(p_outer_layout);
  mpLayout = new QGridLayout();
  p_outer_layout->addLayout(mpLayout);
  p_outer_layout->addStretch();
}

cedar::proc::gui::ToolBox::~ToolBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ToolBox::addItem(const std::string& icon, const std::string& data, const std::string& tooltip)
{
  QToolButton *button = new QToolButton();
  button->setCheckable(true);
  button->setAutoExclusive(true);
  button->setText(icon.c_str());
  button->setProperty("data", QVariant(QString(data.c_str())));
  button->setToolTip(tooltip.c_str());
  int num_children = mpLayout->count();
  int row = (num_children + 1) / mColumns;
  int col = (num_children + 1) % mColumns;
  mpLayout->addWidget(button, row, col);

  button->setFixedSize(32, 32);

  QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(toolButtonToggled(bool)));

  if (this->mpSelectedButton == NULL)
  {
    this->mpSelectedButton = button;
    this->mpSelectedButton->setChecked(true);
  }
}

std::string cedar::proc::gui::ToolBox::getCurrentItemData() const
{
  if (this->mpSelectedButton != NULL)
  {
    return "TODO";
  }
  else
  {
    return "";
  }
}

void cedar::proc::gui::ToolBox::toolButtonToggled(bool checked)
{
  if (checked)
  {
    QToolButton *p_sender = dynamic_cast<QToolButton*>(QObject::sender());
    CEDAR_DEBUG_ASSERT(p_sender != NULL);
    this->mpSelectedButton = p_sender;
    QString data = this->mpSelectedButton->property("data").toString();
    emit selectionChanged(data);
  }
}
