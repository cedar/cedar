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

    File:        GraphicsBase.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 12

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_XXX_XXX_H
#define CEDAR_XXX_XXX_H

// LOCAL INCLUDES
#include "processing/gui/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QGraphicsItem>
#include <QPen>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::GraphicsBase : public QGraphicsItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // static constants
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef unsigned int GraphicsGroup;
  const static GraphicsGroup GRAPHICS_GROUP_NONE = 0;
  const static GraphicsGroup GRAPHICS_GROUP_STEP = 1 << 0;
  const static GraphicsGroup GRAPHICS_GROUP_TRIGGER = 1 << 1;
  const static GraphicsGroup GRAPHICS_GROUP_UNKNOWN = 1 << 16;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GraphicsBase(qreal width, qreal height,
               GraphicsGroup group = GRAPHICS_GROUP_UNKNOWN,
               GraphicsGroup canConnectTo = GRAPHICS_GROUP_NONE);

  //!@brief Destructor
  virtual ~GraphicsBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool canConnectTo(GraphicsBase* pTarget) const;

  bool canConnect() const;

  QPointF getConnectionAnchorInScene() const;

  virtual QPointF getConnectionAnchorRelative() const;

  const GraphicsGroup& getGroup() const;

  QRectF boundingRect() const;

  QPen getOutlinePen() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  qreal mWidth;
  qreal mHeight;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  GraphicsGroup mGroup;
  GraphicsGroup mAllowedConnectTargets;

}; // class cedar::xxx

#endif // CEDAR_XXX_XXX_H

