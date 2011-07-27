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

    File:        Connection.cpp

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
#include "processing/gui/Connection.h"
#include "processing/gui/StepItem.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Connection::Connection(cedar::proc::gui::GraphicsBase *pSource, cedar::proc::gui::GraphicsBase *pTarget)
:
QGraphicsLineItem(pSource),
mpSource(pSource),
mpTarget(pTarget)
{
  pSource->addConnection(this);
  pTarget->addConnection(this);
  this->update();
}

cedar::proc::gui::Connection::~Connection()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Connection::update()
{
  this->setZValue(-1.0);
  QLineF line;

  QPointF pos1(this->mpSource->boundingRect().width()/2.0, this->mpSource->boundingRect().height()/2.0);
  line.setP1(pos1);

  QPointF pos2 = this->mpTarget->scenePos() - this->mpSource->scenePos();
  pos2.rx() += this->mpTarget->boundingRect().width()/2.0;
  pos2.ry() += this->mpTarget->boundingRect().height()/2.0;
  line.setP2(pos2);

  this->setLine(line);
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getSource()
{
  return this->mpSource;
}
cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getTarget()
{
  return this->mpTarget;
}
