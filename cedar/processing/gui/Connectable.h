/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Connectable.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 11 13

    Description: Header file for the class cedar::proc::gui::Connectable.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECTABLE_H
#define CEDAR_PROC_GUI_CONNECTABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/namespace.h"
#include "cedar/processing/DataRole.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QGraphicsSvgItem>

/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::gui::Connectable : public QObject, public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief mapping from data slot names to their graphical representation
  typedef std::map<std::string, cedar::proc::gui::DataSlotItem*> DataSlotNameMap;

  //!@brief mapping from data role id to a map of all data slots fitting this id
  typedef std::map<cedar::proc::DataRole::Id, DataSlotNameMap> DataSlotMap;

  //! Enum-class for the display mode of steps.
  class DisplayMode
  {
    public:
      //! the id of an enum entry
      typedef cedar::aux::EnumId Id;

    public:
      /*! @brief Construct method that fills the enum.
       *  @see cedar::aux::EnumBase
       */
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(ICON_AND_TEXT, "ICON_AND_TEXT", "icon and text"));
        mType.type()->def(cedar::aux::Enum(ICON_ONLY, "ICON_ONLY", "icon only"));
      }

      //! @returns A const reference to the base enum object.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Display icon and text
      static const Id ICON_AND_TEXT = 0;

      //! Display an icon only
      static const Id ICON_ONLY = 1;

    private:
      //! The base enum object.
      static cedar::aux::EnumType<cedar::proc::gui::Connectable::DisplayMode> mType;
  };


protected:
  class Decoration
  {
    public:
      Decoration(Connectable* pConnectable, const QString& icon, const QString& description, const QColor& bg = QColor(255, 255, 255));

      ~Decoration()
      {
        delete mpIcon;
        delete mpRectangle;
      }

      void setPosition(const QPointF& pos);

      void setSize(double sizeFactor);

    private:
      QGraphicsSvgItem* mpIcon;

      QGraphicsRectItem* mpRectangle;

      QString mIconFile;
  };

  CEDAR_GENERATE_POINTER_TYPES(Decoration);


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Connectable(qreal width, qreal height, cedar::proc::gui::GraphicsBase::GraphicsGroup group);

  //!@brief Destructor
  virtual ~Connectable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief return the represented step
  cedar::proc::ConnectablePtr getConnectable();

  //!@brief return the represented step
  cedar::proc::ConstConnectablePtr getConnectable() const;

  //!@brief set the represented step
  void setConnectable(cedar::proc::ConnectablePtr);

  //!@brief resets the internal step
  void resetPointer()
  {
    mConnectable.reset();
  }

  //! Resizes slots that are close to the mouse pointer in connection mode.
  void magnetizeSlots(const QPointF& mousePositionInScene);

  //! Removes all effects of magnetization
  void demagnetizeSlots();

  //! Returns the slot item of the given role and name.
  cedar::proc::gui::DataSlotItem* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief returns one of the data slots associated with this step
  cedar::proc::gui::DataSlotItem const* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name) const;

  //!@brief returns a map of all data slots of the same id
  cedar::proc::gui::Connectable::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role);

  const cedar::proc::gui::Connectable::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void slotAdded(cedar::proc::DataRole::Id role, const std::string& name);

  void slotRemoved(cedar::proc::DataRole::Id role, const std::string& name);

  void addDataItemFor(cedar::proc::DataSlotPtr slot);

  //! Updates the positions of all items attached to this one (decorations and data slots).
  void updateAttachedItems();

  //! Updates the positions of the decoration items.
  void updateDecorationPositions();

  //!@briefs adds graphical representations for all data items
  void addDataItems();

  void sizeChanged();

  //! Assigns an offset for the input and output slots.
  void setInputOutputSlotOffset(qreal offset);

  //! Returns the offset for input and output slots.
  qreal getInputOutputSlotOffset() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Updates the positions of the data slot items.
  void updateDataSlotPositions();

  void itemSelected(bool selected);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The base size of data slots (modified by display mode and other factors).
  static const qreal M_BASE_DATA_SLOT_SIZE;

  //! Amount of padding between data slots.
  static const qreal M_DATA_SLOT_PADDING;

protected:
  //! SvgItem displaying the connectable's icon
  QGraphicsSvgItem* mpIconDisplay;

  //!@brief The current display mode of the step.
  cedar::proc::gui::Connectable::DisplayMode::Id mDisplayMode;

  DecorationPtr mpRecordedDecoration;

  //! The decorations for this connectable.
  std::vector<DecorationPtr> mDecorations;

private:
  //! An offset to be added to in- and output slot positions.
  qreal mInputOutputSlotOffset;

  //!@brief a map of all data slots of the current step
  DataSlotMap mSlotMap;

  //!@brief the represented step
  cedar::proc::ConnectablePtr mConnectable;

  boost::signals2::connection mSlotAddedConnection;
  boost::signals2::connection mSlotRemovedConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::Connectable

#endif // CEDAR_PROC_GUI_CONNECTABLE_H

