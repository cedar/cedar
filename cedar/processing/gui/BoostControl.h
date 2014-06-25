/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        BoostControl.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 06 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_BOOST_CONTROL_H
#define CEDAR_PROC_GUI_BOOST_CONTROL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_BoostControl.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/gui/BoostControl.fwd.h"
#include "cedar/processing/sources/Boost.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <map>


/*!@brief A widget for conveniently controlling the boosts in a network.
 */
class cedar::proc::gui::BoostControl : public QWidget, public Ui_BoostControl
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
  BoostControl();

  //!@brief The destructor.
  ~BoostControl();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Sets the network whose boosts are controlled by this widget.
   */
  void setGroup(cedar::proc::GroupPtr network);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Adds a boost to this control widget.
  void addBoost(cedar::proc::sources::BoostPtr boost);

  //! Translates the boost signal by emitting a Qt signal.
  void translateElementAddedSignal(cedar::proc::ElementPtr);

  //! Translates the boost signal by emitting a Qt signal.
  void translateElementRemovedSignal(cedar::proc::ConstElementPtr);

  void addGroup(cedar::proc::GroupPtr group);

  void disconnectSignals();

  QTreeWidgetItem* getGroupItem(const std::string& elementPath);

  void connectRenameSignal(cedar::proc::ElementPtr element);

signals:
  //! Used to translate a boost signal to a Qt signal.
  void elementAddedSignal(QString);

  //! Used to translate a boost signal to a Qt signal.
  void elementRemovedSignal(QString);

private slots:
  //! Reacts to an element that has been added in the underlying network.
  void elementAdded(QString elementName);

  //! Reacts to the removal of a boost in the underlying network.
  void elementRemoved(QString elementName);

  //! Reacts to the change of the name of a boost in the underlying network.
  void elementNameChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupPtr mGroup;

  std::vector<boost::signals2::connection> mElementAddedConnections;

  std::vector<boost::signals2::connection> mElementRemovedConnections;

  std::map<const cedar::proc::Element*, QString> mElementNames;

}; // class cedar::proc::gui::BoostControl

#endif // CEDAR_PROC_GUI_BOOST_CONTROL_H

