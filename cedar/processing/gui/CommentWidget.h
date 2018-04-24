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

    File:        CommentWidget.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 04 09

    Description: Header for the @em cedar::aux::gui::CommentWidget class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_PROC_GUI_COMMENT_WIDGET_H
#define CEDAR_PROC_GUI_COMMENT_WIDGET_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/gui/CommentWidget.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>

/*!@brief GUI representation for the Comment section.
  */
class cedar::proc::gui::CommentWidget
:
public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The public constructor.
  CommentWidget();

  //!@brief The public constructor.
  CommentWidget(QWidget* pParent);

  //!@brief The public destructor.
  ~CommentWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the obtained step and recreating the widget to set the record parameters.
  void setConnectable(cedar::proc::ConnectablePtr connectable);

  void clear();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the GUI.
  void refreshWidget();

  //!@brief Create the headers for the Widget.
  void createHeader(const std::string& name);

  //!@brief Write the Comment into the text field!
  void fillInCommentSection(std::string& comment);

  //!@brief Resets the widget and its GUI elements.
  void clearLayout();

private slots:
  void updateCommentString();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The step currently displayed.
  cedar::proc::ConnectablePtr mConnectable;

  //!@brief The layout for this widget.
  QVBoxLayout* mMainLayout;

  QTextEdit* mCommentTextField;
};

#endif // CEDAR_PROC_GUI_COMMENT_WIDGET_H
