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

    File:        CouplingCollection.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::gui::CouplingCollection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_COUPLING_COLLECTION_H
#define CEDAR_PROC_GUI_COUPLING_COLLECTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/CouplingCollection.fwd.h"
#include "cedar/processing/gui/Connectable.fwd.h"
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/processing/DataSlot.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"

// SYSTEM INCLUDES
#include <deque>
#include <tuple>
#include <vector>


/*!@brief Displays a collection of couplings between elements in processing architectures as a connection.
 */
class cedar::proc::gui::CouplingCollection : public QObject, public cedar::proc::gui::Connection
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CouplingCollection(cedar::proc::gui::Scene* scene);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Appends the connectable to the ones already contained in this collection.
  void append(cedar::proc::ConnectablePtr connectable, bool update = true);

  //! Appends the connectables from the given collection to the ones already contained in this collection.
  void append(cedar::proc::gui::CouplingCollection* collection, bool update = true);

  //! Prepends the connectable in front of the ones already contained in this collection.
  void prepend(cedar::proc::ConnectablePtr connectable, bool update = true);

  //! Returns all connectables in this collection.
  std::vector<cedar::proc::ConnectablePtr> getContents() const;

  //! Returns all connectables in this collection as configurables.
  std::vector<cedar::aux::ConfigurablePtr> getContentsAsConfigurables() const;

  bool isDeleteable() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void updateSourceAndTargetSlots();

  void componentAdded(bool update);

  cedar::proc::gui::Connectable* findSource();

  std::tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr> findSourceAndSlot();

  std::tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr> findTargetAndSlot();

  cedar::proc::gui::DataSlotItem* findSourceSlot();

  cedar::proc::gui::DataSlotItem* findTargetSlot();

  cedar::proc::DataConnectionPtr getConnection(cedar::proc::DataRole::Id role);

  bool containsSingleGroup() const;

  bool canShowContentsInWindow() const;

  cedar::proc::GroupPtr getSingleGroup() const;

private slots:
  void unhideContents();

  void showContentsInWindow();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::deque<cedar::proc::ConnectableWeakPtr> mComponents;

  cedar::proc::gui::Scene* mpScene;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::CouplingCollection

#endif // CEDAR_PROC_GUI_COUPLING_COLLECTION_H

