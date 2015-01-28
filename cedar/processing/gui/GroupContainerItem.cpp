/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GroupContainerItem.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 01 20

    Description: Source file for the class cedar::proc::gui::GroupContainerItem.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GroupContainerItem.h"
#include "cedar/processing/gui/GroupWidget.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/Group.h"

// SYSTEM INCLUDES
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsSceneDragDropEvent>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupContainerItem::GroupContainerItem(cedar::proc::gui::Group* pGroup)
:
cedar::proc::gui::GraphicsBase(400, 300, GRAPHICS_GROUP_UNKNOWN, GRAPHICS_GROUP_NONE, BASE_SHAPE_RECT),
mpContainer(new QGraphicsProxyWidget(this)),
mpGroupWidget(new cedar::proc::gui::GroupWidget(pGroup))
{
  auto outer = new QWidget();
  mpContainer->setWidget(outer);
  auto layout = new QVBoxLayout();
  layout->setContentsMargins(2, 2, 2 ,2);
  outer->setLayout(layout);

  auto titlebar_layout = new QHBoxLayout();
  layout->addLayout(titlebar_layout);
  auto p_close_button = new QPushButton("close");
  titlebar_layout->addStretch(1);
  titlebar_layout->addWidget(p_close_button, 0);

  layout->addWidget(this->mpGroupWidget);

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

bool cedar::proc::gui::GroupContainerItem::canDuplicate() const
{
  return false;
}

bool cedar::proc::gui::GroupContainerItem::canResize() const
{
  return true;
}

void cedar::proc::gui::GroupContainerItem::sizeChanged()
{
  this->mpContainer->setGeometry(QRectF(0, 0, this->width(), this->height()));
}
