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

    File:        ElementTreeWidget.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Header file for the class cedar::proc::gui::ElementTreeWidget.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ELEMENT_TREE_WIDGET_H
#define CEDAR_PROC_GUI_ELEMENT_TREE_WIDGET_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupPath.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ElementTreeWidget.fwd.h"
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"

// SYSTEM INCLUDES
#include <QTreeWidget>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
  #include <boost/function.hpp>
#endif // Q_MOC_RUN
#include <map>
#include <string>
#include <vector>


/*!@brief A widget for displaying (a filtered subset of) the elements in a cedar::proc::Group.
 */
class cedar::proc::gui::ElementTreeWidget : public QTreeWidget
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
  ElementTreeWidget(QWidget* pParent = nullptr);

  //!@brief Destructor
  virtual ~ElementTreeWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the group whose simulation is controlled by this widget.
  void setGroup(cedar::proc::GroupPtr group);

  //! Sets the filter for elements. Only (non-group) elements for which the filter returns true will be added to the tree.
  void setFilter(const boost::function<bool(cedar::proc::ConstElementPtr)>& filter);

  //! Sets the column into which the name of the element is written.
  void setNameColumn(int column)
  {
    this->mNameColumn = column;
  }

  //! Returns the name column.
  int getNameColumn() const
  {
    return this->mNameColumn;
  }

  //! If set to true, the widget allows editing the names of the items it displays.
  void setNameEditingEnabled(bool enabled);

  //! Returns the group path stored for the item.
  std::string getPathFromItem(QTreeWidgetItem* pItem) const;

  //! Sets the group path stored in the item.
  void setItemPath(QTreeWidgetItem* pItem, const std::string& newPath);

  //! Returns the item corresponding to the given element.
  QTreeWidgetItem* findItemForElement(cedar::proc::ConstElementPtr element);

signals:
  //! Used to translate a boost signal to a Qt signal.
  void elementAddedSignal(QString);

  //! Used to translate a boost signal to a Qt signal.
  void elementRemovedSignal(QString);

  //! Emitted whenever the name of an element changes. The integer passed along is the row of the changed element.
  void elementNameChanged(QTreeWidgetItem* item);

  //--------------------------------------------------------------------------------------------------------------------
  // boost signals
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_SIGNAL(ElementAdded, void(QTreeWidgetItem* item, cedar::proc::ElementPtr));

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void translateElementAddedSignal(cedar::proc::ElementPtr);

  void translateElementRemovedSignal(cedar::proc::ConstElementPtr);

  void connectRenameSignal(cedar::proc::ElementPtr element);

  void disconnectSignals();

  void addGroup(cedar::proc::GroupPtr group);

  void addElement(cedar::proc::ElementPtr element);

  QTreeWidgetItem* getGroupItem(const std::string& elementPath);

  QTreeWidgetItem* findItemWithPath(const cedar::proc::GroupPath& path);

private slots:
  //! Reacts to an element that has been added in the underlying group.
  void elementAdded(QString elementName);

  //! Reacts to the removal of a boost in the underlying group.
  void elementRemoved(QString elementName);

  //! Reacts to the change of the name of an element in the underlying group.
  void elementNameChanged();

  //! Takes care of applying a new name, if the edit is such a change.
  void itemChanged(QTreeWidgetItem* pItem, int column);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupPtr mGroup;

  std::map<const cedar::proc::Element*, QString> mElementNames;

  std::vector<boost::signals2::connection> mElementAddedConnections;

  std::vector<boost::signals2::connection> mElementRemovedConnections;

  boost::function<bool(cedar::proc::ConstElementPtr)> mFilter;

  int mNameColumn;

  bool mNameEditingEnabled;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ElementTreeWidget

#endif // CEDAR_PROC_GUI_ELEMENT_TREE_WIDGET_H

