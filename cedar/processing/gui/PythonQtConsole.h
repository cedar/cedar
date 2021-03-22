

/*
 *
 *  Copyright (C) 2010 MeVis Medical Solutions AG All Rights Reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: MeVis Medical Solutions AG, Universitaetsallee 29,
 *  28359 Bremen, Germany or:
 *
 *  http://www.mevis.de
 *
 */

//----------------------------------------------------------------------------------
/*!
// \file    PythonQtConsole.h
// \author  Florian Link
// \author  Last changed by $Author: florian $
// \date    2006-10
*/
//----------------------------------------------------------------------------------

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifndef _PythonQtConsole_H
#define _PythonQtConsole_H

// FORWARD DECLARATIONS
#include "cedar/processing/gui/PythonQtConsole.fwd.h"

#include "cedar/auxiliaries/Log.h"
#include "cedar/configuration.h"
#include "cedar/processing/gui/CodeWidget.h"
#include <cedar/auxiliaries/Singleton.h>
#include <cedar/processing/GroupDeclaration.h>
#include <cedar/processing/ElementDeclaration.h>

#include "PythonQt.h"
#include "Scene.h"
#include "CoPYObject.h"
#include <QVariant>
#include <QTextEdit>
#include <QGraphicsScene>

class QCompleter;

//-------------------------------------------------------------------------------
//! A simple console for python scripting
class PythonQtConsole : public cedar::proc::gui::CodeWidgetScope::CodeEditor
{
Q_OBJECT



public:
  PythonQtConsole(QWidget* parent, Qt::WindowFlags i = 0);

  ~PythonQtConsole();

public Q_SLOTS:
  //! execute current line
  void executeLine();

  void executeCode(const QString& code);

  //! derived key press event
  void keyPressEvent (QKeyEvent * e);

  //! output from console
  void consoleMessage(const QString & message, bool error);

  //! output redirection
  void stdOut(const QString& s);
  //! output redirection
  void stdErr(const QString& s);

  void insertCompletion(const QString&);

public:
  //! returns true if python cerr had an error
  bool hadError() { return _hadError; }

  //! returns true if python cerr had an error
  void clearError() {
    _hadError = false;
  }
  void setScene(cedar::proc::gui::Scene *pScene);

  void reset();

  void addVariable(const QString &name, const QVariant& variable);

  QStringList getVariables();
protected:
  //! handle the pressing of tab
  void handleTabCompletion();

  //! Returns the position of the command prompt
  int commandPromptPosition();

  //! Sets the current font
  void setCurrentFont(const QColor& color = QColor(0,0,0), bool bold = false);

  //! flush output that was not yet printed
  void flushStdOut();

private:

  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

  cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
  cedar::proc::gui::CoPYObject *mpPy;

  PythonQtObjectPtr mpContext;

  QString _clickedAnchor;
  QString _storageKey;
  QString _commandPrompt;

  QString _currentMultiLineCode;

  QString _stdOut;
  QString _stdErr;

  QTextCharFormat _defaultTextCharacterFormat;
  QCompleter* _completer;

  cedar::proc::gui::Scene* mpScene;

  bool _hadError;


};



#endif

