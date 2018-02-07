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

    File:        DefaultConnectableIconView.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Source file for the class cedar::proc::gui::DefaultConnectableIconView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/DefaultConnectableIconView.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include <QSvgRenderer>

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DefaultConnectableIconView::DefaultConnectableIconView()
:
mpIconDisplay(nullptr)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::DefaultConnectableIconView::connectableChanged()
{
  auto class_id = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getConnectable());
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(class_id));
  auto elem_decl = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(class_id);

  this->setIconPath(elem_decl->determinedIconPath());
}

void cedar::proc::gui::DefaultConnectableIconView::setIconPath(const QString& path)
{
  // initial position/scale
  qreal pos_x, pos_y;
  pos_x = pos_y = 0;
  qreal scale = 1.0;

  if (this->mpIconDisplay)
  {
    //Use the renderer load function instead of deleting and creating objects
    this->mpIconDisplay->renderer()->load(path);
  }
  else
  {
    // create icon
    this->mpIconDisplay = new QGraphicsSvgItem(path, this);
    this->mpIconDisplay->setPos(pos_x, pos_y);
    this->mpIconDisplay->setScale(scale);
    this->mpIconDisplay->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  }
}

void cedar::proc::gui::DefaultConnectableIconView::prepareSvgExport()
{
  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIconDisplay->setCacheMode(QGraphicsItem::NoCache);
}

void cedar::proc::gui::DefaultConnectableIconView::unprepareSvgExport()
{
  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIconDisplay->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void cedar::proc::gui::DefaultConnectableIconView::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
  // do nothing, this item is not visible itself, only the icon is drawn
}

QRectF cedar::proc::gui::DefaultConnectableIconView::boundingRect() const
{
  if (this->mpIconDisplay == nullptr)
  {
    return QRectF(0, 0, 0, 0);
  }
  else
  {
    return this->mpIconDisplay->boundingRect();
  }
}

void cedar::proc::gui::DefaultConnectableIconView::setBounds(const qreal& x, const qreal& y, const qreal& size)
{
  if (this->mpIconDisplay == nullptr)
  {
    return;
  }

  this->mpIconDisplay->setPos(x, y);
  qreal w = this->mpIconDisplay->boundingRect().width();
  qreal h = this->mpIconDisplay->boundingRect().width();
  qreal major = std::max(w, h);
  this->mpIconDisplay->setScale(size / major);
}
