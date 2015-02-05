/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ElementClassList.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ELEMENT_CLASS_LIST_H
#define CEDAR_PROC_GUI_ELEMENT_CLASS_LIST_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ElementClassList.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <QListWidget>
#include <string>
#include <vector>

/*!@brief A widget showing a list of steps that can be dragged into the architecture area.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::ElementClassList : public QListWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ElementClassList(QWidget *pParent = NULL);

  //!@brief Destructor
  ~ElementClassList();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief for a given category, show all registered steps (their icon and name)
  void showList(const std::string& categoryName);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Opens up the context menu.
  void contextMenuEvent(QContextMenuEvent* pEvent);

  //! What drop actions are supported by this widget.
  Qt::DropActions supportedDropActions() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void addListEntry
       (
         const std::string& className,
         const std::string& fullClassName,
         const QIcon& icon,
         const std::vector<QString>& decorations,
         const std::string& description,
         const std::string& deprecation,
         const std::string& source,
         cedar::aux::ConstPluginDeclarationPtr declaration
       );

private slots:
  void showDeprecatedSteps(bool show);

  void rebuild();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::string mCategoryName;

}; // class ElementClassList

#endif // CEDAR_PROC_GUI_ELEMENT_CLASS_LIST_H
