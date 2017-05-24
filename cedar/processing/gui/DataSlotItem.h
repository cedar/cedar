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

    File:        DataSlotItem.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 15

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_DATA_SLOT_ITEM_H
#define CEDAR_PROC_GUI_DATA_SLOT_ITEM_H

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/DataSlot.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/DataSlotItem.fwd.h"

// SYSTEM INCLUDES
#include <QObject>

/*!@brief User-Interface representation of a cedar::proc::DataSlot.
 *
 *        This class implements a cedar::proc::gui::GraphicsBase that is used to display a cedar::proc::DataSlot inside
 *        a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::DataSlotItem : public QObject, public cedar::proc::gui::GraphicsBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param pParent The parent item to which the data slot belongs.
   * @param slot    The slot displayed by this item.
   */
  DataSlotItem(cedar::proc::gui::GraphicsBase *pParent, cedar::proc::DataSlotPtr slot);

  //!@brief Destructor.
  ~DataSlotItem();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Implements the drawing of the slot.
   */
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  /*!@brief Creates the context menu for the item.
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  /*!@brief Connects the UI item and the underlying data slot to the target.
   */
  cedar::proc::gui::Connection* connectTo(cedar::proc::gui::DataSlotItem *pTarget);

  /*!@brief Returns the name of the data slot.
   */
  const std::string& getName() const;

  /*!@brief Returns a const pointer to the data slot displayed by this item.
   */
  cedar::proc::ConstDataSlotPtr getSlot() const;

  /*!@brief Returns a pointer to the data slot displayed by this item.
   */
  cedar::proc::DataSlotPtr getSlot();

  /*!@brief Returns whether this slot can connect to somewhere else.
   *
   * @returns False, if the slot is already connected (and not a collection), true otherwise.
   */
  bool canConnect() const;

  /*!@brief Checks, whether the slot can be connected to the target.
   */
  cedar::proc::gui::ConnectValidity canConnectTo(GraphicsBase* pTarget) const;

  //! Sets the scaling factor of the slot that occurs due to mouse interaction.
  void setMagneticScale(qreal scale)
  {
    this->mMagneticScale = scale;
  }

  //! Returns how much the slot is currently scaled due to mouse interaction.
  qreal getMagneticScale() const
  {
    return this->mMagneticScale;
  }

  //! Sets whether the connections of this item are highlighted due to one of its owners being selected.
  void setHighlightedBySelection(bool highlight);

  bool canDuplicate() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! creates a tool tip if the mouse cursor hovers over this object and highlights the connections from this slot.
  void hoverEnterEvent(QGraphicsSceneHoverEvent* pEvent);

  //! Removes the highlighting of the connections from this slot.
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* pEvent);

signals:
  //! Signal that is emitted whenever the validity of attached connections changes.
  void connectionValidityChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Generates a tool tip depending on the content of DataSlot.
   */
  void generateTooltip();

  cedar::proc::gui::ConnectValidity translateValidity(cedar::proc::DataSlot::VALIDITY validity) const;

  void translateValidityChangedSignal();

private slots:
  void updateConnectionValidity();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The step item that this slot belongs to.
  cedar::proc::gui::GraphicsBase* mpStep;

  //! The slot itself.
  cedar::proc::DataSlotPtr mSlot;

  //!@brief a connection to a signal emitted if validity of the data slot changes
  boost::signals2::connection mSlotConnection;

  //! How much the slot is currently being scaled due to attraction by the mouse pointer.
  qreal mMagneticScale;
}; // class DataSlotItem

#endif // CEDAR_PROC_GUI_DATA_SLOT_ITEM_H
