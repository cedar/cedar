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

    File:        ToolBox.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_TOOLBOX_H
#define CEDAR_PROC_GUI_TOOLBOX_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ToolBox.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <map>
#include <string>


/*!@brief This is a base class for widgets that contain little boxes that represent some tools.
 */
class cedar::proc::gui::ToolBox : public QWidget
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
  ToolBox(unsigned int columns = 5, QWidget *pParent = NULL);

  //!@brief Destructor
  ~ToolBox();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief add a tool to the toolbox, represented by an icon
  void addItem(const std::string& icon, const std::string& data, const std::string& tooltip);

  //!@brief add a tool to the toolbox, represented by a simple string name
  void addStringItem(const std::string& icon, const std::string& data, const std::string& tooltip);

  //!@brief Return the data of the current item.
  std::string getCurrentItemData() const;

  //!@brief set a specific tool mode
  void selectMode(const std::string& mode);

public slots:
  //!@brief handles a button press in the toolbox
  void toolButtonToggled(bool checked);

signals:
  //!@brief a signal, which is emitted if the current tool is changed
  void selectionChanged(QString data);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief handles the resize event
  void resizeEvent(QResizeEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief refresh the column layout
  void relayout();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief number of columns in the widget
  unsigned int mColumns;
  //!@brief size of a button in the widget
  unsigned int mButtonSize;
  //!@brief the layout of the toolbox
  QGridLayout *mpLayout;
  //!@brief the currently selected button
  QToolButton *mpSelectedButton;
  //!@brief map of all buttons in the widget
  std::map<std::string, QToolButton*> mButtons;

}; // class cedar::proc::gui::ToolBox

#endif // CEDAR_PROC_GUI_TOOLBOX_H
