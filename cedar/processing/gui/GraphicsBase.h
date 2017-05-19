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

// CEDAR INCLUDES
#include "cedar/processing/gui/Connection.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/gui/Connection.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/ResizeHandle.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsItem>
#include <QPen>
#include <vector>


/*!@brief Base class for graphical items in a processing scene.
 *
 *        This class realizes a lot of the functionality related to drawing of processing steps, triggers etc. in a
 *        cedar::proc::Scene. Because it is configurable, it also provides methods for storing and restoring the display
 *        item from a configuration file.
 */
class cedar::proc::gui::GraphicsBase : public QGraphicsItem, public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::Scene;

  //--------------------------------------------------------------------------------------------------------------------
  // static constants
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a group a GraphicsBase can belong to
  typedef unsigned int GraphicsGroup;
  //!@brief group NONE
  const static GraphicsGroup GRAPHICS_GROUP_NONE = 0;
  //!@brief group STEP
  const static GraphicsGroup GRAPHICS_GROUP_STEP = 1 << 0;
  //!@brief group TRIGGER
  const static GraphicsGroup GRAPHICS_GROUP_TRIGGER = 1 << 1;
  //!@brief group DATA ITEM
  const static GraphicsGroup GRAPHICS_GROUP_DATA_ITEM = 1 << 2;
  //!@brief group for NETWORK
  const static GraphicsGroup GRAPHICS_GROUP_GROUP = 1 << 3;
  //!@brief group UNKNOWN
  const static GraphicsGroup GRAPHICS_GROUP_UNKNOWN = 1 << 16;

  //!@brief enum of highlight modes for GraphicsBase
  enum HighlightMode
  {
    HIGHLIGHTMODE_NONE,
    HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET,
    HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING,
    HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR,
    HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER,
    HIGHLIGHTMODE_GROUP_MEMBER_LEAVING,
    HIGHLIGHTMODE_SEARCH_RESULT
  };

  //!@brief enum  of base shapes for GraphicsBase
  enum BaseShape
  {
    BASE_SHAPE_RECT,
    BASE_SHAPE_ROUNDED_RECT,
    BASE_SHAPE_ROUND,
    BASE_SHAPE_DIAMOND,
    BASE_SHAPE_CROSS
  };


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GraphicsBase(qreal width, qreal height,
               GraphicsGroup group = GRAPHICS_GROUP_UNKNOWN,
               GraphicsGroup canConnectTo = GRAPHICS_GROUP_NONE,
               BaseShape shape = BASE_SHAPE_ROUNDED_RECT);

  //!@brief Destructor
  virtual ~GraphicsBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief for a given GraphicsBase object, check if this instance can connect to it
  virtual cedar::proc::gui::ConnectValidity canConnectTo(GraphicsBase* pTarget) const;

  //!@brief return flag if this GraphicsBase can - in general - connect to other GraphicsBase instances
  virtual bool canConnect() const;

  //!@brief snap point for connection line in scene coordinates
  QPointF getConnectionAnchorInScene() const;

  //!@brief snap point for connection line in relative coordinates (relative to the GraphicsBase's origin)
  virtual QPointF getConnectionAnchorRelative() const;

  //!@brief to which graphics group does this GraphicsBase belong?
  const GraphicsGroup& getGroup() const;

  //!@brief bounding box of this GraphicsBase
  QRectF boundingRect() const;

  //!@brief get pen of the shape (e.g. to look up thickness of shape line)
  QPen getOutlinePen() const;

  //! Returns the brush used for filling the item's shape
  QBrush getOutlineBrush() const;

  //!@brief This method highlights this item according to how it can connect to the source.
  void highlightConnectionTarget(cedar::proc::gui::GraphicsBase *pConnectionSource);

  //!@brief set the highlight mode (used after evaluating canConnectTo of another GraphicsBase
  void setHighlightMode(HighlightMode mode);

  //!@brief get the current highlight mode
  HighlightMode getHighlightMode() const;

  //!@brief width of this GraphicsBase
  qreal width() const
  {
    return static_cast<qreal>(this->mWidth->getValue());
  }

  //!@brief height of this GraphicsBase
  qreal height() const
  {
    return static_cast<qreal>(this->mHeight->getValue());
  }

  //!@brief set height of this GraphicsBase
  void setHeight(qreal height);

  //!@brief set width of this GraphicsBase
  void setWidth(qreal width);

  //! Sets the size (width and height) of the object.
  void setSize(qreal width, qreal height);

  //!@brief Sets the bounds of the item.
  void setBounds(const QRectF& rect);

  //!@brief add a Connection to this GraphicsBase
  void addConnection(Connection* pConnection);

  /*! @brief Removes the connection from the list of connections managed by this graphics item.
   *
   *         This means that the start and end positions of the connection are no longer updated when this
   *         item is moved around.
   */
  void removeConnection(Connection* pConnection);

  //!@brief Removes all connections from the list of connections managed by this graphics item.
  void removeAllConnections();

  //!@brief Checks whether a GUI connection to the target exists in this item's connection list.
  bool hasGuiConnectionTo(GraphicsBase const* pTarget) const;

  //!\brief overwrite this function if your customized graphics item needs to disconnect some children items
  virtual void disconnect();

  //!\brief overwrite this function if your customized graphics item needs to disconnect a children items
  virtual void disconnect(cedar::proc::gui::GraphicsBase* pTarget);

  //!@brief read configuration from a node (e.g. the position in a scene)
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief write configuration to a node (e.g. the position in a scene)
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //!@brief redraw the connections if GraphicsBase instance is moved around
  void updateConnections();

  //!@brief return the number of connections
  unsigned int getNumberOfConnections();

  //!@brief set outline fill color
  void setOutlineColor(const QColor& color);

  //!@brief set fill color
  void setFillColor(const QColor& color);

  //! Returns the color used to fill the base shape.
  QColor getFillColor() const;

  //! Returns the currently set fill style.
  Qt::BrushStyle getFillStyle() const;

  /*!@brief Draw the default graphical representation.
   */
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  //! Can be implemented to react to changes of the items size.
  virtual void sizeChanged();

  //! Returns the brush used for highlighting potential target groups.
  static QBrush getTargetGroupBrush();

  //! Returns the brush used for highlighting when one of the child items is potentially leaving. :(
  static QBrush getLeavingGroupBrush();

  /*! Can be overridden in inheriting classes in order to upadte the item's tooltip before it is displayed.
   */
  virtual void updateToolTip();

  //!@brief sets the "read only"-ness
  virtual void setReadOnly(bool readOnly);

  //!@brief returns whether this graphics base is read-only
  bool isReadOnly() const
  {
    return this->mReadOnly;
  }

  //! If the function returns true, this item will request manual confirmation from the user in case it is being deleted.
  virtual bool manualDeletionRequiresConfirmation() const;

  //! Returns the vector of in- and outgoing (gui) connections for this (gui) element.
  std::vector<Connection*>& getConnections()
  {
    return this->mConnections;
  }

  //! Specifies whether the item can be duplicated.
  virtual bool canDuplicate() const = 0;

  //! If this returns false, the item should not be dragged around.
  virtual bool canBeDragged() const;

  //! Returns the minimum size that this graphics object must have.
  const QSizeF getMinimumSize() const;

  /*! Returns the color used to fill the foreground when the given brush is not solid.
   */
  static QColor nonsolidBrushForegroundColor(QBrush brush);

  /*! Returns the color used to fill the background when the given brush is not solid.
   */
  static QColor nonsolidBrushBackgroundColor(QBrush brush);

  /*! Fills the given pixmap in the same way that the background of a graphicsbase would be filled with the given brush.
   */
  static void paintBackgroundColor(QPixmap& pixmap, QBrush brush);

  //!@brief get the right color for a certain validity
  static const QColor& getValidityColor(cedar::proc::gui::ConnectValidity validity);

  //--------------------------------------------------------------------------------------------------------------------
  // public signals
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_SIGNAL(FillColorChanged, void(QColor));

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief paint this GraphicsBase instance
  void paintFrame(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  //!@brief deal with changes to this instance (e.g. position changes)
  QVariant itemChange(GraphicsItemChange change, const QVariant & value);

  //! Called whenever the item is added to a scene.
  virtual void itemSceneHasChanged();

  /*!@brief Sets the base shape of the item.
   *
   * @remarks This method should only be called during construction.
   */
  void setBaseShape(BaseShape shape);

  //! Sets the snap-to-grid property of the element.
  inline void setSnapToGrid(bool snap)
  {
    this->mSnapToGrid = snap;
  }

  //! Sets an override fill style. If an override fill style is set, it will be used instead of the normal fill style.
  void setOverrideFillStyle(Qt::BrushStyle style, bool update = true);

  //! Removes the override fill style, reverting the item to using its normal fill style.
  void unsetOverrideFillStyle(bool update = true);

  //! Sets an override fill style. If an override fill style is set, it will be used instead of the normal fill style.
  void setOverrideFillColor(const QColor& color, bool update = true);

  //! Removes the override fill style, reverting the item to using its normal fill style.
  void unsetOverrideFillColor(bool update = true);

  //! Sets the fill style
  void setFillStyle(Qt::BrushStyle style, bool update = true);

  //! Set whether or not this item is resizeable.
  void setResizeable(bool resizeable);

  /*! Emits a changed event in the item's scene (if the item has a scene).
   *
   * @param alwaysEmit The event is always emmited, even for items that usually don't emit it.
   */
  void emitChangedEventToScene(bool alwaysEmit = false);

  //! If this returns true, the item can be resized.
  virtual bool canResize() const;

  //! Updates the display of the resize handles.
  void updateResizeHandles();

  //! Sets the minimum size for this graphics object.
  void setMinimumSize(QSizeF size);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void itemSelectedChanged(bool selected);

  void updateResizeHandles(bool show);

  void clearResizeHandles();
  
  //! Called whenever the item has been selected or deselected.
  virtual void itemSelected(bool selected);

  //! Draws the base shape of the item on the given painter
  void drawShape(QPainter* painter);

  //! For non-basic shapes (i.e., any that require mPath to be valid), updates the points on the path.
  void updateShape();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief color for state "valid"
  static const QColor mValidityColorValid;
  //!@brief color for state "warning"
  static const QColor mValidityColorWarning;
  //!@brief color for state "error"
  static const QColor mValidityColorError;
  //!@brief color for state "unknown"
  static const QColor mValidityColorUnknown;

  //! Color for groups being left
  static const QColor mColorGroupBeingLeft;

  //! Color for search results
  static const QColor mColorSearchResult;

  //! Color for target groups
  static const QColor mColorTargetGroup;
  
  //!@brief color for outline
  static const QColor mDefaultOutlineColor;

  //!@brief fill color
  static const QColor mDefaultFillColor;

protected:
  //!@brief flag if the background should be drawn
  bool mDrawBackground;

private:
  //!@brief the highlight mode
  cedar::proc::gui::GraphicsBase::HighlightMode mHighlightMode;

  //!@brief the shape of this GraphicsBase
  BaseShape mShape;

  //!@brief the current outline color
  QColor mOutlineColor;

  //!@brief the current fill color
  QColor mFillColor;

  //!@brief Override fill color.
  boost::optional<QColor> mOverrideFillColor;

  //! Brush style used for filling the shape.
  Qt::BrushStyle mFillStyle;

  //! Override fill style.
  boost::optional<Qt::BrushStyle> mOverrideFillStyle;

  //!@brief The path used for drawing this shape.
  QPainterPath mPath;

  //!@brief Whether the item snaps to the grid.
  bool mSnapToGrid;

  //! Whether or not resizing is allowed.
  bool mResizeable;

  //! Whether the item is in read-only mode
  bool mReadOnly;

  //! Resize handle.
  std::vector<cedar::proc::gui::ResizeHandle*> mpResizeHandles;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief width of the GraphicsBase
  cedar::aux::DoubleParameterPtr mWidth;

  //!@brief height of the GraphicsBase
  cedar::aux::DoubleParameterPtr mHeight;

  QSizeF mMinimumSize;

  //!@brief group of this instance
  GraphicsGroup mGroup;

  //!@brief all groups this instance can connect to
  GraphicsGroup mAllowedConnectTargets;

  //!@brief vector of connections
  std::vector<Connection*> mConnections;

  static const QSizeF M_MINIMUM_SIZE;

}; // class cedar::proc::gui::GraphicsBase

#endif // CEDAR_PROC_GUI_GRAPHICS_BASE_H

