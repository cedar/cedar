/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// LOCAL INCLUDES
#include "processing/gui/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 *
 * @todo Calculate the number of columns based on the widget size and a provided button width.
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
  void addItem(const std::string& icon, const std::string& data, const std::string& tooltip);
  void addStringItem(const std::string& icon, const std::string& data, const std::string& tooltip);
  std::string getCurrentItemData() const;

  void selectMode(const std::string& mode);

public slots:
  void toolButtonToggled(bool checked);

signals:
  void selectionChanged(QString data);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void resizeEvent(QResizeEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void relayout();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  unsigned int mColumns;
  unsigned int mButtonSize;
  QGridLayout *mpLayout;
  QToolButton *mpSelectedButton;
  std::map<std::string, QToolButton*> mButtons;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ToolBox

#endif // CEDAR_PROC_GUI_TOOLBOX_H

