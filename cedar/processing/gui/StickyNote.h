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

    File:        StickyNote.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 07 01

    Description: Header of the @em cedar::proc::gui::StickyNote class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STICKYNODE_H_
#define STICKYNODE_H_

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"


// SYSTEM INCLUDES
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QPainter>
#include <QWidget>
#include <QTextEdit>
#include <QGraphicsProxyWidget>

//!@brief A GUI class that creates a pane where the user can write a comment or note.
class cedar::proc::gui::StickyNote: public QGraphicsItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:
  //!@brief the constructor
  StickyNote(int x, int y, QGraphicsScene* pParent);

  //!@brief the destructor
  ~StickyNote();


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Returns the most outer rect of this element.
  QRectF boundingRect() const;

  //!@brief Paints the sticky note.
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  //!@brief Checks if an key was pressed.
  void keyPressEvent(QKeyEvent* event);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief The most outer rect.
  QRectF bound;

  //!@brief The text inside the note.
  QTextEdit text;

  //!@brief A helper for storing QWidgets in a QGraphicsItem .
  QGraphicsProxyWidget *mpProxy;

  //!@brief The color of the note.
  QColor color;

  //!@brief A reference to the parent scene.
  QGraphicsScene* pParent;
};

#endif /* CEDAR_PROC_STICKYNODE_H_ */
