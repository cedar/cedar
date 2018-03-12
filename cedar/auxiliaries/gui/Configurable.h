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

    File:        Configurable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_CONFIGURABLE_H
#define CEDAR_AUX_GUI_CONFIGURABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/Configurable.fwd.h"
#include "cedar/auxiliaries/gui/Parameter.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"
#include "cedar/auxiliaries/Parameter.fwd.h"
#include "cedar/auxiliaries/ObjectListParameter.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN
#include <QWidget>
#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <QSignalMapper>
#include <map>
#include <string>
#include <set>
#include <vector>


/*!@brief A widget to display and manipulate the parameters of cedar::aux::Configurables.
 *
 * @remarks This widget is intended to replace the old PropertyPane in the long run.
 */
class cedar::aux::gui::Configurable : public QWidget
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class DataDelegate;

  class CustomTree : public QTreeWidget
  {
    public:
      QTreeWidgetItem* getItemFromIndex(const QModelIndex& index)
      {
        return this->itemFromIndex(index);
      }
  };

  //!@cond SKIPPED_DOCUMENTATION
  class ParameterItem;
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Configurable(QWidget* pParent = NULL);

  //! Destructor.
  ~Configurable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the parameters & configurable children for a given configurable.
   *
   * @param configurable The configurable to display.
   * @param readOnly     If true, the configurable cannot be edited, only displayed.
   */
  void display(cedar::aux::ConfigurablePtr configurable, bool readOnly = false);

  /*!@brief Clears the widget and removes any displayed parameters.
   */
  void clear();

  /*!@brief Displays a list of configurables.
   */
  void display(std::vector<cedar::aux::ConfigurablePtr> configurables);

  //! Returns the parameter for the given model index
  cedar::aux::ParameterPtr parameterFromModelIndex(const QModelIndex& index);

public slots:
  //! Resizes the rows to fit their contents.
  void fitRowsToContents();


signals:
  //! Emitted, whenever a parameter in the widget changes.
  void settingsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Appends a root configurable.
  void appendRootConfigurable(cedar::aux::ConfigurablePtr configurable);

  //! Appends the given configurable to the tree widget item.
  void append(size_t configurableIndex, cedar::aux::ConfigurablePtr configurable, QTreeWidgetItem* pItem, const std::string& pathSoFar);

  //! Appends the given parameter to the tree node.
  void append(size_t configurableIndex, cedar::aux::ParameterPtr parameter, QTreeWidgetItem* pNode, const std::string& pathSoFar);

  QTreeWidgetItem* appendHeading(QTreeWidgetItem* pParent, const QString& text, int hLevel = 2);

  void makeHeading(QTreeWidgetItem* item, const QString& text, int hLevel = 2);

  void updateChangeState(QTreeWidgetItem* item, cedar::aux::Parameter* pParameter, bool firstUpdate);

  void updateLinkState(QTreeWidgetItem* item, cedar::aux::Parameter* pParameter);

  QTreeWidgetItem* getItemForParameter(cedar::aux::Parameter* parameter);

  QString getPathFromItem(QTreeWidgetItem* item);

  void disconnect(QTreeWidgetItem* item);

  void appendObjectListParameter
       (
         size_t configurableIndex,
         cedar::aux::ParameterPtr objectListParameter,
         QTreeWidgetItem* pParent,
         const std::string& path
       );

  void translateParameterAddedSignal(unsigned int configurableIndex, cedar::aux::ParameterPtr parameter);

  void translateParameterRemovedSignal(unsigned int configurableIndex, cedar::aux::ParameterPtr parameter);

  void translateParameterNameChangedSignal(unsigned int configurableIndex, const std::string& oldName, const std::string& newName);

  int getRootIndexForItem(QTreeWidgetItem* pItem) const;

signals:
  //! Emitted, whenever a parameter is added to the displayed configurable.
  void parameterAdded(int configurableIndex, QString path);

  //! Emitted, whenever a parameter is removed from the displayed configurable.
  void parameterRemoved(int configurableIndex, QVariant parameter);

  //! Emitted, whenever a parameter in the displayed configurable is renamed.
  void parameterRenamed(int configurableIndex, QString oldName, QString newName);

  void delButtonClickedSignal(const QString &fullPath);

private slots:
  void parameterChangeFlagChanged();

  void objectParameterValueChanged();

  void objectListParameterValueChanged();

  void parameterAddedSlot(int configurableIndex, QString path);

  void parameterRemovedSlot(int configurableIndex, QVariant parameter);

  void parameterRenamedSlot(int configurableIndex, QString oldName, QString newName);

  void handleDeleteButtonClicked(QString fullPath);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  CustomTree* mpPropertyTree;

  std::vector<cedar::aux::ConfigurablePtr> mDisplayedConfigurables;

  boost::signals2::scoped_connection mParameterAddedConnection;

  boost::signals2::scoped_connection mParameterRemovedConnection;

  std::map<cedar::aux::Parameter*, boost::signals2::connection> mParameterRenamedConnections;

  QSignalMapper* delButtonSignalMapper;
}; // class cedar::aux::gui::Configurable

/*!@brief Internal class for putting the correct widget types into the QTreeWidget used by
 *        cedar::aux::gui::Configurable.
 *
 * @see the documentation for QTreeWidget and Qt's Model/View concept for details on data delegates.
 * @remarks This is an internal class of the Configurable widget. Not intended for use outside of it. It is only in the
 *          header because it needs to be processed by the meta-object compiler.
 */
class cedar::aux::gui::Configurable::DataDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  //! Constructor for the DataDelegate.
  DataDelegate(cedar::aux::gui::Configurable* configurableWidget, bool readOnly = false);

  //! Destructor.
  ~DataDelegate();

  //! This method decides which type of widget should be opened for which kind of parameter.
  QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem& option, const QModelIndex &index) const;

public slots:
  //! Reacts to when a widget in the tree is being destroyed.
  void widgetDestroyed(QObject* removed);

private:
  cedar::aux::gui::Configurable* mConfigurableWidget;

  // set storing all opened editors - during the destructor call, all remaining editors are deleted
  mutable std::set<QObject*> mOpenedEditors;

  bool mReadOnly;
};

#endif // CEDAR_AUX_GUI_CONFIGURABLE_H

