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

    File:        WidgetContainerItem.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 01 29

    Description: Source file for the class cedar::proc::gui::WidgetContainerItem.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/WidgetContainerItem.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::WidgetContainerItem::WidgetContainerItem()
:
cedar::proc::gui::GraphicsBase(400, 300, GRAPHICS_GROUP_UNKNOWN, GRAPHICS_GROUP_NONE, BASE_SHAPE_RECT),
mpContainer(new QGraphicsProxyWidget(this)),
mpContained(nullptr),
mpTitleWidget(nullptr)
{
  auto style = QApplication::style();
  auto close_icon = style->standardIcon(QStyle::SP_TitleBarCloseButton);

  auto outer = new QWidget();
  this->mpContainer->setWidget(outer);
  auto layout = new QVBoxLayout();
  layout->setContentsMargins(2, 2, 2 ,2);
  outer->setLayout(layout);

  auto titlebar_base = new QWidget();
  this->mpTitleBarLayout = new QHBoxLayout();
  this->mpTitleBarLayout->setContentsMargins(0, 0, 0, 0);
  titlebar_base->setLayout(this->mpTitleBarLayout);
  layout->addWidget(titlebar_base);

  // close button
  auto p_close_button = new QPushButton(close_icon, "");
  QSize size = close_icon.availableSizes().at(0);
  size.setWidth(size.width() + 2);
  size.setHeight(size.height() + 2);
  p_close_button->setFixedSize(size);

  // title bar construction
  this->mpTitleBarLayout->addStretch(1);
  this->mpTitleBarLayout->addWidget(p_close_button, 0);

  QObject::connect(p_close_button, SIGNAL(clicked()), this, SLOT(deleteLater()));
  this->setResizeable(true);
  this->setFlag(QGraphicsItem::ItemIsMovable, true);
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
  this->setMinimumSize(QSizeF(400,300));
  this->updateResizeHandles();
  this->sizeChanged();

  this->setAcceptDrops(true);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::WidgetContainerItem::setWidget(QWidget* widget)
{
  auto layout = this->mpContainer->widget()->layout();
  CEDAR_DEBUG_ASSERT(layout != nullptr);

  // if set, remove the previous widget from the layout
  if (this->mpContained)
  {
    layout->removeWidget(this->mpContained);
  }

  // set the new widget
  this->mpContained = widget;
  layout->addWidget(this->mpContained);
}

bool cedar::proc::gui::WidgetContainerItem::canDuplicate() const
{
  return false;
}

bool cedar::proc::gui::WidgetContainerItem::canResize() const
{
  return true;
}

void cedar::proc::gui::WidgetContainerItem::sizeChanged()
{
  this->mpContainer->setGeometry(QRectF(0, 0, this->width(), this->height()));
}

void cedar::proc::gui::WidgetContainerItem::setTitle(const std::string& title)
{
  if (this->mpTitleWidget == nullptr)
  {
    this->mpTitleWidget = new QLabel("");
    this->mpTitleBarLayout->insertWidget(0, this->mpTitleWidget);
  }

  this->mpTitleWidget->setText(QString::fromStdString(title));
}
