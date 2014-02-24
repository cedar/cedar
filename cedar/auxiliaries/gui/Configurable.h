/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include <QWidget>
#include <QTreeWidget>


/*!@brief A widget to display and manipulate the parameters of cedar::aux::Configurables.
 *
 * @remarks This widget is intended to replace the old cedar::aux/proc::PropertyPane in the long run.
 */
class cedar::aux::gui::Configurable : public QWidget
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class DataDelegate;

  class ParameterItem;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Configurable(QWidget* pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the parameters & configurable children for a given configurable.
   *
   * @param configurable The configurable to display.
   */
  void display(cedar::aux::ConfigurablePtr configurable);

  /*!@brief Clears the widget and removes any displayed parameters.
   */
  void clear();

public slots:
  void fitRowsToContents();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Appends the given configurable to the tree widget item.
  void append(cedar::aux::ConfigurablePtr configurable, QTreeWidgetItem* pItem, const std::string& pathSoFar);

  //! Appends the given parameter to the tree node.
  void append(cedar::aux::ParameterPtr parameter, QTreeWidgetItem* pNode, const std::string& pathSoFar);

  QTreeWidgetItem* appendHeading(QTreeWidgetItem* pParent, const QString& text, int hLevel = 2);

  void makeHeading(QTreeWidgetItem* item, const QString& text, int hLevel = 2);

  void updateChangeState(QTreeWidgetItem* item, cedar::aux::Parameter* pParameter);

  QTreeWidgetItem* getItemForParameter(cedar::aux::Parameter* parameter);

  QString getPathFromItem(QTreeWidgetItem* item);

  void disconnect(QTreeWidgetItem* item);

  void appendObjectListParameter
       (
         cedar::aux::ObjectListParameterPtr objectListParameter,
         QTreeWidgetItem* pParent,
         const std::string& path
       );

private slots:
  void parameterChangeFlagChanged();

  void objectParameterValueChanged();

  void objectListParameterValueChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QTreeWidget* mpPropertyTree;

}; // class cedar::aux::gui::Configurable

#endif // CEDAR_AUX_GUI_CONFIGURABLE_H

