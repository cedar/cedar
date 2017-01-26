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

    File:        GraphicsBase.cpp

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

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/ResizeHandle.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/processing/gui/Settings.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <iostream>
#include <limits.h>

const QColor cedar::proc::gui::GraphicsBase::mValidityColorValid(170, 218, 24);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorWarning(255, 207, 40);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorError(206, 0, 11);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorUnknown(0, 76, 249);

const QColor cedar::proc::gui::GraphicsBase::mColorGroupBeingLeft = QColor::fromRgb(200, 200, 200);
const QColor cedar::proc::gui::GraphicsBase::mColorTargetGroup = QColor::fromRgb(125, 125, 255);

const QColor cedar::proc::gui::GraphicsBase::mColorSearchResult = QColor::fromRgb(0, 127, 255);

const QColor cedar::proc::gui::GraphicsBase::mDefaultOutlineColor(Qt::black);
const QColor cedar::proc::gui::GraphicsBase::mDefaultFillColor(Qt::white);

const QSizeF cedar::proc::gui::GraphicsBase::M_MINIMUM_SIZE(static_cast<qreal>(50.0), static_cast<qreal>(20.0));

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GraphicsBase::GraphicsBase(qreal width,
                                             qreal height,
                                             GraphicsGroup group,
                                             GraphicsGroup canConnectTo,
                                             BaseShape shape)
:
mDrawBackground(true),
mHighlightMode(HIGHLIGHTMODE_NONE),
mOutlineColor(cedar::proc::gui::GraphicsBase::mDefaultOutlineColor),
mFillColor(cedar::proc::gui::GraphicsBase::mDefaultFillColor),
mFillStyle(Qt::SolidPattern),
mSnapToGrid(true),
mResizeable(false),
mReadOnly(false),
mWidth
(
  new cedar::aux::DoubleParameter
      (
        this, "width", 120.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
      )
),
mHeight
(
  new cedar::aux::DoubleParameter
      (
        this, "height", 50.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
      )
),
mMinimumSize(M_MINIMUM_SIZE),
mGroup(group),
mAllowedConnectTargets(canConnectTo)
{
  this->setZValue(1.0);

  this->mWidth->setValue(width);
  this->mHeight->setValue(height);

  this->setFlags(this->flags() | QGraphicsItem::ItemSendsGeometryChanges);

  this->setBaseShape(shape);
}

cedar::proc::gui::GraphicsBase::~GraphicsBase()
{
  this->disconnect();

  while (!this->mConnections.empty())
  {
    // on destruction, connections remove themselves from their graphics base, thus, the connection will automatically
    // be removed from this->mConnections
    delete *this->mConnections.begin();
  }

  for (auto resize_handle : this->mpResizeHandles)
  {
    delete resize_handle;
  }
  this->mpResizeHandles.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::gui::GraphicsBase::canBeDragged() const
{
  return false;
}

bool cedar::proc::gui::GraphicsBase::manualDeletionRequiresConfirmation() const
{
  return false;
}

void cedar::proc::gui::GraphicsBase::emitChangedEventToScene(bool alwaysEmit)
{
  bool is_data_slot_item = ((this->mGroup & GRAPHICS_GROUP_DATA_ITEM) > 0);
  if ((alwaysEmit || !is_data_slot_item) && this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->emitSceneChanged();
  }
}

void cedar::proc::gui::GraphicsBase::setReadOnly(bool readOnly)
{
  this->mReadOnly = readOnly;

  if (readOnly)
  {
    this->clearResizeHandles();
  }
}

void cedar::proc::gui::GraphicsBase::itemSceneHasChanged()
{
  // empty default implementation
}

void cedar::proc::gui::GraphicsBase::setResizeable(bool resizeable)
{
  this->mResizeable = resizeable;

  this->clearResizeHandles();
}

void cedar::proc::gui::GraphicsBase::sizeChanged()
{
  // nothing to do
}

void cedar::proc::gui::GraphicsBase::updateToolTip()
{
  // empty default implementation
}

void cedar::proc::gui::GraphicsBase::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  this->paintFrame(painter, style, widget);
}

void cedar::proc::gui::GraphicsBase::setBaseShape(BaseShape shape)
{
  this->mShape = shape;

  this->updateShape();
}

void cedar::proc::gui::GraphicsBase::updateShape()
{
  mPath = QPainterPath();
  switch (this->mShape)
  {
    case BASE_SHAPE_DIAMOND:
      mPath.moveTo(0, this->height() / static_cast<qreal>(2));
      mPath.lineTo(this->width() / static_cast<qreal>(2), this->height());
      mPath.lineTo(this->width(), this->height() / static_cast<qreal>(2));
      mPath.lineTo(this->width() / static_cast<qreal>(2), 0);
      mPath.lineTo(0, this->height() / static_cast<qreal>(2));
      break;

    case BASE_SHAPE_CROSS:
      mPath.moveTo(0, this->height());
      mPath.lineTo(this->width(), 0);
      mPath.moveTo(0, 0);
      mPath.lineTo(this->width(), this->height());
      break;

    case BASE_SHAPE_ROUNDED_RECT:
    case BASE_SHAPE_RECT:
    case BASE_SHAPE_ROUND:
      // no custom path is used
      break;

    default:
      CEDAR_THROW(cedar::aux::UnhandledValueException, "Unhandled shape in cedar::proc::gui::GraphicsBase::updateShape()");
  }
}

void cedar::proc::gui::GraphicsBase::setBounds(const QRectF& rect)
{
  const QRectF& current_bounds = this->boundingRect();
  qreal offset_w = current_bounds.width() - this->width();
  qreal offset_h = current_bounds.height() - this->height();
  this->mWidth->setValue(rect.width() - offset_w);
  this->mHeight->setValue(rect.height() - offset_h);
  // shift the position by the change from the old to the new bounding rect
  this->setPos(this->pos() - this->boundingRect().topLeft() + rect.topLeft());
//  this->update();
  this->sizeChanged();
}

void cedar::proc::gui::GraphicsBase::setHeight(qreal height)
{
  this->mHeight->setValue(static_cast<double>(height));
  this->emitChangedEventToScene();
  this->update();
  this->sizeChanged();
}

void cedar::proc::gui::GraphicsBase::setWidth(qreal width)
{
  this->mWidth->setValue(static_cast<double>(width));
  this->emitChangedEventToScene();
  this->update();
  this->sizeChanged();
}

void cedar::proc::gui::GraphicsBase::setSize(qreal width, qreal height)
{
  this->mHeight->setValue(static_cast<double>(height));
  this->mWidth->setValue(static_cast<double>(width));
  this->update();
  this->sizeChanged();
}

void cedar::proc::gui::GraphicsBase::setOutlineColor(const QColor& color)
{
  this->mOutlineColor = color;
  this->update();
}

void cedar::proc::gui::GraphicsBase::setFillColor(const QColor& color)
{
  this->mFillColor = color;
  this->update();
}

QColor cedar::proc::gui::GraphicsBase::getFillColor() const
{
  return this->mFillColor;
}

Qt::BrushStyle cedar::proc::gui::GraphicsBase::getFillStyle() const
{
  return this->mFillStyle;
}


void cedar::proc::gui::GraphicsBase::setFillStyle(Qt::BrushStyle style, bool update)
{
  this->mFillStyle = style;

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::setOverrideFillStyle(Qt::BrushStyle style, bool update)
{
  this->mOverrideFillStyle = style;

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::unsetOverrideFillStyle(bool update)
{
  this->mOverrideFillStyle.reset();

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::setOverrideFillColor(const QColor& color, bool update)
{
  this->mOverrideFillColor = color;

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::unsetOverrideFillColor(bool update)
{
  this->mOverrideFillColor.reset();

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::highlightConnectionTarget(cedar::proc::gui::GraphicsBase *pConnectionSource)
{
  switch (pConnectionSource->canConnectTo(this))
  {
    case cedar::proc::gui::CONNECT_YES:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET);
      break;

    case cedar::proc::gui::CONNECT_WARNING:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING);
      break;

    case cedar::proc::gui::CONNECT_ERROR:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR);
      break;

    case cedar::proc::gui::CONNECT_NO:
    default:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
  }
}

void cedar::proc::gui::GraphicsBase::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::readConfiguration(node);

  qreal x = 0, y = 0;
  try
  {
    x = node.get<qreal>("positionX");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // ok, x not set
  }
  try
  {
    y = node.get<qreal>("positionY");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // ok, y not set
  }

  bool snap_to_grid = this->mSnapToGrid;
  this->mSnapToGrid = false;
  this->setPos(x, y);
  this->mSnapToGrid = snap_to_grid;
}

void cedar::proc::gui::GraphicsBase::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  this->cedar::aux::Configurable::writeConfiguration(root);

  root.put("positionX", this->pos().x());
  root.put("positionY", this->pos().y());
}

bool cedar::proc::gui::GraphicsBase::hasGuiConnectionTo(GraphicsBase const* pTarget) const
{
  for (size_t i = 0; i < this->mConnections.size(); ++i)
  {
    cedar::proc::gui::Connection* p_connection = this->mConnections.at(i);
    if (p_connection->getSource() == this && p_connection->getTarget() == pTarget)
    {
      return true;
    }
  }

  return false;
}

void cedar::proc::gui::GraphicsBase::addConnection(cedar::proc::gui::Connection* pConnection)
{
  this->emitChangedEventToScene(true); // we always emit a changed event when a connection is added
  this->mConnections.push_back(pConnection);
}

void cedar::proc::gui::GraphicsBase::removeConnection(cedar::proc::gui::Connection* pConnection)
{
  std::vector<Connection*>::iterator it;
  for (it = mConnections.begin(); it != mConnections.end(); it++)
  {
    if (*it == pConnection)
    {
      break;
    }
  }
  // found something, delete it now!
  if (it != mConnections.end())
  {
    mConnections.erase(it);
  }

  this->emitChangedEventToScene();
}

void cedar::proc::gui::GraphicsBase::removeAllConnections()
{
  // first, disconnect the underlying structures
  this->disconnect();
  // then, delete the graphical representation of the connections
  std::vector<cedar::proc::gui::Connection*> delete_later;
  for (std::vector<cedar::proc::gui::Connection*>::iterator it = mConnections.begin(); it != mConnections.end(); ++it)
  {
    if ((*it)->getSource() == this)
    {
      (*it)->getTarget()->removeConnection(*it);
    }
    else if ((*it)->getTarget() == this)
    {
      (*it)->getSource()->removeConnection(*it);
    }
    // delete own connections later
    delete_later.push_back(*it);
    delete *it;
  }
  // delete all own connections
  for (size_t i = 0; i < delete_later.size(); ++i)
  {
    this->removeConnection(delete_later.at(i));
  }
  this->mConnections.clear();
}

cedar::proc::gui::GraphicsBase::HighlightMode cedar::proc::gui::GraphicsBase::getHighlightMode() const
{
  return this->mHighlightMode;
}

void cedar::proc::gui::GraphicsBase::setHighlightMode(cedar::proc::gui::GraphicsBase::HighlightMode mode)
{
  this->mHighlightMode = mode;
  this->update();
}

QRectF cedar::proc::gui::GraphicsBase::boundingRect() const
{
  return QRectF(QPointF(0, 0), QSizeF(this->width(), this->height()));
}

bool cedar::proc::gui::GraphicsBase::canConnect() const
{
  return this->mAllowedConnectTargets != GRAPHICS_GROUP_NONE;
}

cedar::proc::gui::ConnectValidity cedar::proc::gui::GraphicsBase::canConnectTo(GraphicsBase* pTarget) const
{
  if ((this->mAllowedConnectTargets & pTarget->mGroup) != 0)
  {
    if (pTarget == this)
    {
      return cedar::proc::gui::CONNECT_NO;
    }
    else
    {
      return cedar::proc::gui::CONNECT_YES;
    }
  }
  return cedar::proc::gui::CONNECT_NO;
}

QPointF cedar::proc::gui::GraphicsBase::getConnectionAnchorInScene() const
{
  return this->scenePos() + this->getConnectionAnchorRelative();
}

QPointF cedar::proc::gui::GraphicsBase::getConnectionAnchorRelative() const
{
  return QPointF(this->boundingRect().width()/2.0, this->boundingRect().height()/2.0);
}

const cedar::proc::gui::GraphicsBase::GraphicsGroup& cedar::proc::gui::GraphicsBase::getGroup() const
{
  return this->mGroup;
}

QBrush cedar::proc::gui::GraphicsBase::getTargetGroupBrush()
{
  QBrush brush;
  brush.setColor(mColorTargetGroup);
  brush.setStyle(Qt::BDiagPattern);
  return brush;
}

QBrush cedar::proc::gui::GraphicsBase::getLeavingGroupBrush()
{
  QBrush brush;
  brush.setColor(mColorGroupBeingLeft);
  brush.setStyle(Qt::BDiagPattern);
  return brush;
}

QBrush cedar::proc::gui::GraphicsBase::getOutlineBrush() const
{
  switch (this->mHighlightMode)
  {
    case HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER:
      return cedar::proc::gui::GraphicsBase::getTargetGroupBrush();

    case HIGHLIGHTMODE_GROUP_MEMBER_LEAVING:
      return cedar::proc::gui::GraphicsBase::getLeavingGroupBrush();

    default:
    case HIGHLIGHTMODE_NONE:
    {
      QBrush brush;
      QColor color = this->mFillColor;
      if (this->mOverrideFillColor)
      {
        color = this->mOverrideFillColor.get();
      }

      if (this->mReadOnly)
      {
        if (color.hsvHue() == -1)
        {
          color = QColor::fromHsv(color.hsvHue(), color.hsvSaturation(), color.value() - 30);
        }
        else
        {
          color = QColor::fromHsv(color.hsvHue(), color.hsvSaturation() / 2, color.value());
        }
      }
      brush.setColor(color);
      if (this->mOverrideFillStyle)
      {
        brush.setStyle(this->mOverrideFillStyle.get());
      }
      else
      {
        brush.setStyle(this->mFillStyle);
      }
      return brush;
    }
  }
}

QPen cedar::proc::gui::GraphicsBase::getOutlinePen() const
{
  QPen pen;
  if (this->isSelected())
  {
    pen.setStyle(Qt::DashLine);
  }

  if (this->mHighlightMode != HIGHLIGHTMODE_NONE)
  {
    pen.setWidthF(2);
  }
  // determine what pen to use for highlighting
  switch (this->mHighlightMode)
  {
    case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET:
      pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorValid);
      break;

    case HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER:
      pen.setColor(cedar::proc::gui::GraphicsBase::mColorTargetGroup);
      break;

    case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR:
      pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorError);
      break;

    case HIGHLIGHTMODE_GROUP_MEMBER_LEAVING:
      pen.setColor(cedar::proc::gui::GraphicsBase::mColorGroupBeingLeft);
      break;

    case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING:
      pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorWarning);
      break;

    case HIGHLIGHTMODE_SEARCH_RESULT:
      pen.setColor(cedar::proc::gui::GraphicsBase::mColorSearchResult);
      break;

    default:
    case HIGHLIGHTMODE_NONE:
      pen.setColor(this->mOutlineColor);
  }

  return pen;
}

QColor cedar::proc::gui::GraphicsBase::nonsolidBrushBackgroundColor(QBrush brush)
{
  return brush.color();
}

QColor cedar::proc::gui::GraphicsBase::nonsolidBrushForegroundColor(QBrush /* brush */)
{
  return QColor(Qt::white);
}


void cedar::proc::gui::GraphicsBase::paintBackgroundColor(QPixmap& pixmap, QBrush brush)
{
  if (brush.style() != Qt::SolidPattern)
  {
    pixmap.fill(nonsolidBrushBackgroundColor(brush));
  }
  QPainter painter(&pixmap);
  QBrush foreground = brush;
  if (brush.style() != Qt::SolidPattern)
  {
    foreground.setColor(nonsolidBrushForegroundColor(brush));
  }
  painter.fillRect(QRect(0, 0, pixmap.width(), pixmap.height()), foreground);
}

void cedar::proc::gui::GraphicsBase::paintFrame(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  // draw the base shape
  if (mDrawBackground)
  {
    painter->save();
    QBrush brush = this->getOutlineBrush();

    // for non-solid patterns, we draw a white background
    if (brush.style() != Qt::SolidPattern)
    {
      QColor bg_color = nonsolidBrushBackgroundColor(brush);
      QBrush solid_background(bg_color);
      painter->setBrush(solid_background);
      QPen invisible_pen(Qt::NoPen);
      painter->setPen(invisible_pen);
      this->drawShape(painter);

      QBrush foreground = brush;
      foreground.setColor(nonsolidBrushForegroundColor(brush));
      painter->setBrush(foreground);
    }
    else
    {
      painter->setBrush(brush);
    }
    painter->setPen(this->getOutlinePen());

    this->drawShape(painter);

    painter->restore();
  }
}

void cedar::proc::gui::GraphicsBase::drawShape(QPainter* painter)
{
  const qreal roundedness = static_cast<qreal>(4);
  QRectF bounds(QPointF(0, 0), QSizeF(this->width(), this->height()));

  // draw the base shape
  if (mDrawBackground)
  {
    switch (this->mShape)
    {
      case BASE_SHAPE_ROUNDED_RECT:
        painter->drawRoundedRect(bounds, roundedness, roundedness);
        break;

      case BASE_SHAPE_RECT:
        painter->drawRect(bounds);
        break;

      case BASE_SHAPE_ROUND:
        painter->drawEllipse(bounds);
        break;

      case BASE_SHAPE_DIAMOND:
      case BASE_SHAPE_CROSS:
        //!@todo This really only needs to be called if the shape changes.
        this->updateShape();
        painter->drawPath(mPath);
        break;
    }
  }
}


const QColor& cedar::proc::gui::GraphicsBase::getValidityColor(ConnectValidity validity)
{
  switch (validity)
  {
    case cedar::proc::gui::CONNECT_YES:
      return mValidityColorValid;

    case cedar::proc::gui::CONNECT_NO:
    case cedar::proc::gui::CONNECT_ERROR:
      return mValidityColorError;

    case cedar::proc::gui::CONNECT_WARNING:
      return mValidityColorWarning;

    case cedar::proc::gui::CONNECT_UNKNOWN:
      return mValidityColorUnknown;

    default:
      return mValidityColorError;
  }
}

void cedar::proc::gui::GraphicsBase::itemSelected(bool)
{
  // empty default implementation
}

void cedar::proc::gui::GraphicsBase::itemSelectedChanged(bool selected)
{
  // if the item was deselected, it is not part of the list of selected items; thus, remove its handles
  if (!selected)
  {
    this->updateResizeHandles(false);
  }

  this->itemSelected(selected);
}

QVariant cedar::proc::gui::GraphicsBase::itemChange(GraphicsItemChange change, const QVariant & value)
{
  qreal grid_size = cedar::proc::gui::SettingsSingleton::getInstance()->getSnapGridSize();
  switch (change)
  {
    case QGraphicsItem::ItemPositionChange:
    {
      QPointF new_pos = value.toPointF();

      if
      (
        this->mSnapToGrid
        && this->scene()
        && cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->getSnapToGrid()
      )
      {
        new_pos.rx() = cedar::aux::math::round(new_pos.x() / grid_size) * grid_size;
        new_pos.ry() = cedar::aux::math::round(new_pos.y() / grid_size) * grid_size;
      }

      return new_pos;
    }

    case QGraphicsItem::ItemPositionHasChanged:
    {
      this->updateConnections();

      for (size_t i = 0; i < this->mpResizeHandles.size(); ++i)
      {
        this->mpResizeHandles.at(i)->updatePosition();
      }

      if (this->mGroup != GRAPHICS_GROUP_DATA_ITEM && this->mGroup != GRAPHICS_GROUP_NONE)
      {
        this->emitChangedEventToScene();
      }
      break;
    }
    
    case QGraphicsItem::ItemSelectedHasChanged:
    {
      this->itemSelectedChanged(value.toBool());
      break;
    }

    case QGraphicsItem::ItemSceneHasChanged:
    {
      this->itemSceneHasChanged();
      break;
    }

    default:
      break;
  }
  return QGraphicsItem::itemChange(change, value);
}

void cedar::proc::gui::GraphicsBase::updateResizeHandles()
{
  bool show = this->isSelected();
  if (this->scene() && this->scene()->selectedItems().count() > 1)
  {
    show = false;
  }
  this->updateResizeHandles(show);
}

void cedar::proc::gui::GraphicsBase::updateResizeHandles(bool show)
{
  if (this->canResize() && show)
  {
    for (size_t i = 0; i < cedar::proc::gui::ResizeHandle::directions().size(); ++i)
    {
      this->mpResizeHandles.push_back
      (
        new cedar::proc::gui::ResizeHandle
        (
          this,
          this->mpResizeHandles,
          cedar::proc::gui::ResizeHandle::directions().at(i)
        )
      );
    }
  }
  else
  {
    this->clearResizeHandles();
  }
}

void cedar::proc::gui::GraphicsBase::clearResizeHandles()
{
  for (size_t i = 0; i < this->mpResizeHandles.size(); ++i)
  {
    delete this->mpResizeHandles.at(i);
  }
  this->mpResizeHandles.clear();
}

bool cedar::proc::gui::GraphicsBase::canResize() const
{
  // currently, only networks can be resized
  return this->mResizeable && !this->mReadOnly;
}

void cedar::proc::gui::GraphicsBase::updateConnections()
{
  QList<QGraphicsItem*> children = this->childItems();
  for (int i = 0; i < children.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* child = dynamic_cast<cedar::proc::gui::GraphicsBase*>(children[i]))
    {
      child->updateConnections();
    }
  }
  for (size_t i = 0; i < this->mConnections.size(); ++i)
  {
    this->mConnections.at(i)->update();
  }
}

void cedar::proc::gui::GraphicsBase::disconnect(cedar::proc::gui::GraphicsBase*)
{
}

void cedar::proc::gui::GraphicsBase::disconnect()
{
  // empty default implementation
}

unsigned int cedar::proc::gui::GraphicsBase::getNumberOfConnections()
{
  return this->mConnections.size();
}

void cedar::proc::gui::GraphicsBase::setMinimumSize(QSizeF size)
{
  this->mMinimumSize = size;
}

const QSizeF cedar::proc::gui::GraphicsBase::getMinimumSize() const
{
  return this->mMinimumSize;
}
