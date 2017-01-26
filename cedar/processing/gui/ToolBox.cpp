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

// CEDAR INCLUDES
#include "cedar/processing/gui/ToolBox.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QVariant>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QResizeEvent>
#include <QSpacerItem>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ToolBox::ToolBox(unsigned int columns, QWidget *pParent)
:
QWidget(pParent),
mColumns(columns),
mButtonSize(32),
mpSelectedButton(NULL)
{
  QVBoxLayout *p_outer_layout = new QVBoxLayout();
  p_outer_layout->setContentsMargins(3, 3, 3, 3);
  this->setLayout(p_outer_layout);

  this->mpLayout = new QGridLayout();
  p_outer_layout->addLayout(this->mpLayout, 1);

  p_outer_layout->addStretch();
}

cedar::proc::gui::ToolBox::~ToolBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ToolBox::resizeEvent(QResizeEvent * /* pEvent */)
{
  QRect rect = this->mpLayout->contentsRect();
  int effective_area = rect.width();
  int per_item_width = static_cast<int>(mButtonSize) + this->mpLayout->spacing();
  this->mColumns = effective_area / per_item_width;

  if (this->mColumns == 0)
  {
    this->mColumns = 1;
  }

  this->relayout();
}

void cedar::proc::gui::ToolBox::relayout()
{
  QList<QWidget*> widgets;
  while (this->mpLayout->count() > 0)
  {
    QWidget *p_item = this->mpLayout->takeAt(0)->widget();
    if (p_item != NULL)
    {
      widgets.append(p_item);
    }
  }

  for (int i = 0; i < widgets.size(); ++i)
  {
    QWidget *p_child = widgets.at(i);
    int row = i / mColumns;
    int col = i % mColumns;
    this->mpLayout->addWidget(p_child, row, col);
  }
}

void cedar::proc::gui::ToolBox::addItem(const std::string& icon, const std::string& data, const std::string& tooltip)
{
  QToolButton *button = new QToolButton();
  button->setCheckable(true);
  button->setAutoExclusive(true);
  button->setIcon(QIcon(icon.c_str()));
  button->setProperty("data", QVariant(QString(data.c_str())));
  button->setToolTip(tooltip.c_str());
  int num_children = mpLayout->count();
  int row = num_children / mColumns;
  int col = num_children % mColumns;
  mpLayout->addWidget(button, row, col);

  button->setMaximumSize(static_cast<int>(mButtonSize), static_cast<int>(mButtonSize));
  button->setMinimumHeight(static_cast<int>(mButtonSize));

  // check for duplicates
  CEDAR_ASSERT(mButtons.find(data) == mButtons.end());

  mButtons[data] = button;

  QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(toolButtonToggled(bool)));

  if (this->mpSelectedButton == NULL)
  {
    this->mpSelectedButton = button;
    this->mpSelectedButton->setChecked(true);
  }
}

void cedar::proc::gui::ToolBox::addStringItem(const std::string& icon, const std::string& data, const std::string& tooltip)
{
  QToolButton *button = new QToolButton();
  button->setCheckable(true);
  button->setAutoExclusive(true);
  button->setText(icon.c_str());
  button->setProperty("data", QVariant(QString(data.c_str())));
  button->setToolTip(tooltip.c_str());
  int num_children = mpLayout->count();
  int row = num_children / mColumns;
  int col = num_children % mColumns;
  mpLayout->addWidget(button, row, col);

  button->setFixedSize(32, 32);

  // check for duplicates
  CEDAR_ASSERT(mButtons.find(data) == mButtons.end());

  mButtons[data] = button;

  QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(toolButtonToggled(bool)));

  if (this->mpSelectedButton == NULL)
  {
    this->mpSelectedButton = button;
    this->mpSelectedButton->setChecked(true);
  }
}

void cedar::proc::gui::ToolBox::selectMode(const std::string& mode)
{
  if (this->mButtons.find(mode) == this->mButtons.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "The button \"" + mode + "\" is not known.");
  }
  this->mButtons.find(mode)->second->setChecked(true);
}

std::string cedar::proc::gui::ToolBox::getCurrentItemData() const
{
  if (this->mpSelectedButton != NULL)
  {
    return "NOT IMPLEMENTED";
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
