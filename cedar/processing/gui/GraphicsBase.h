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

#ifndef CEDAR_PROC_GUI_GRAPHICS_BASE_H
#define CEDAR_PROC_GUI_GRAPHICS_BASE_H

// LOCAL INCLUDES
#include "processing/gui/namespace.h"
#include "processing/gui/Connection.h"
#include "auxiliaries/Configurable.h"
#include "auxiliaries/NumericParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QGraphicsItem>
#include <QPen>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::GraphicsBase : public QGraphicsItem, public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // static constants
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef unsigned int GraphicsGroup;
  const static GraphicsGroup GRAPHICS_GROUP_NONE = 0;
  const static GraphicsGroup GRAPHICS_GROUP_STEP = 1 << 0;
  const static GraphicsGroup GRAPHICS_GROUP_TRIGGER = 1 << 1;
  const static GraphicsGroup GRAPHICS_GROUP_GROUP = 1 << 2;
  const static GraphicsGroup GRAPHICS_GROUP_DATA_ITEM = 1 << 3;
  const static GraphicsGroup GRAPHICS_GROUP_UNKNOWN = 1 << 16;

  enum HighlightMode
  {
    HIGHLIGHTMODE_NONE,
    HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET,
    HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER
  };

  enum BaseShape
  {
    BASE_SHAPE_RECT,
    BASE_SHAPE_ROUND
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GraphicsBase(qreal width, qreal height,
               GraphicsGroup group = GRAPHICS_GROUP_UNKNOWN,
               GraphicsGroup canConnectTo = GRAPHICS_GROUP_NONE,
               BaseShape shape = BASE_SHAPE_RECT);

  //!@brief Destructor
  virtual ~GraphicsBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  virtual bool canConnectTo(GraphicsBase* pTarget) const;

  virtual bool canConnect() const;

  QPointF getConnectionAnchorInScene() const;

  virtual QPointF getConnectionAnchorRelative() const;

  const GraphicsGroup& getGroup() const;

  QRectF boundingRect() const;

  QPen getOutlinePen() const;

  void setHighlightMode(HighlightMode mode);

  HighlightMode getHighlightMode() const;

  qreal width() const
  {
    return static_cast<qreal>(this->mWidth->get());
  }

  qreal height() const
  {
    return static_cast<qreal>(this->mHeight->get());
  }

  void addConnection(Connection* pConnection);

  void removeConnection(Connection* pConnection);

  void removeAllConnections();

  //!\brief overwrite this function if your customized graphics item needs to disconnect some children items
  virtual void disconnect();

  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  void saveConfiguration(cedar::aux::ConfigurationNode& root);

  void updateConnections();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void paintFrame(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  QVariant itemChange(GraphicsItemChange change, const QVariant & value);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  bool mDrawBackground;
private:
  cedar::proc::gui::GraphicsBase::HighlightMode mHighlightMode;
  BaseShape mShape;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  cedar::aux::DoubleParameterPtr mWidth;
  cedar::aux::DoubleParameterPtr mHeight;

  GraphicsGroup mGroup;
  GraphicsGroup mAllowedConnectTargets;

  std::vector<Connection*> mConnections;

}; // class cedar::proc::gui::GraphicsBase

#endif // CEDAR_PROC_GUI_GRAPHICS_BASE_H

