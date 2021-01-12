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
// \file    PythonQtConsole.cpp
// \author  Florian Link
// \author  Last changed by $Author: florian $
// \date    2006-10
*/
//----------------------------------------------------------------------------------

#include "PythonQtConsole.h"
#include "ElementList.h"
#include "Group.h"


#include <QMenu>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
#include <QCompleter>
#include <QStringListModel>
#include <QScrollBar>
#include <QAbstractItemView>

//-----------------------------------------------------------------------------

PythonQtConsole::PythonQtConsole(QWidget* parent, const PythonQtObjectPtr& context, Qt::WindowFlags windowFlags)
:
cedar::proc::gui::CodeWidgetScope::CodeEditor(parent)
{
  setWindowFlags(windowFlags);

  _defaultTextCharacterFormat = currentCharFormat();
  _context                    = context;
  _hadError                   = false;

  _completer = new QCompleter(this);
  _completer->setWidget(this);
  QObject::connect(_completer, SIGNAL(activated(const QString&)),
                   this, SLOT(insertCompletion(const QString&)));

  clear();

  connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
  connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdErr(const QString&)));
  insertPlainText("#Controller imported as py\n#Drop steps here for creation and further instructions\nimport numpy as np");
}

//-----------------------------------------------------------------------------

void PythonQtConsole::stdOut(const QString& s)
{
  _stdOut += s;
  int idx;
  while ((idx = _stdOut.indexOf('\n'))!=-1) {
    consoleMessage(_stdOut.left(idx), false);
    std::cout << QStringToPythonConstCharPointer(_stdOut.left(idx)) << std::endl;
    _stdOut = _stdOut.mid(idx+1);
  }
}

void PythonQtConsole::stdErr(const QString& s)
{
  _hadError = true;
  _stdErr += s;
  int idx;
  while ((idx = _stdErr.indexOf('\n'))!=-1) {
    consoleMessage(_stdErr.left(idx), true);
    std::cerr << QStringToPythonConstCharPointer(_stdErr.left(idx)) << std::endl;
    _stdErr = _stdErr.mid(idx+1);
  }
}

void PythonQtConsole::flushStdOut()
{
  if (!_stdOut.isEmpty()) {
    stdOut("\n");
  }
  if (!_stdErr.isEmpty()) {
    stdErr("\n");
  }
}

//-----------------------------------------------------------------------------

PythonQtConsole::~PythonQtConsole() {
}





//-----------------------------------------------------------------------------

void PythonQtConsole::executeLine()
{

  QString code = this->toPlainText();

  // i don't know where this trailing space is coming from, blast it!
  if (code.endsWith(" ")) {
    code.truncate(code.length()-1);
  }

  if (!code.isEmpty()) {

    _currentMultiLineCode = code + "\n";
    executeCode(_currentMultiLineCode);
    _currentMultiLineCode = "";

  }

}

void PythonQtConsole::executeCode(const QString& code)
{
  // put visible cursor to the end of the line
  QTextCursor cursor = QPlainTextEdit::textCursor();
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);

  int cursorPosition = this->textCursor().position();

  // evaluate the code
  _stdOut = "";
  _stdErr = "";
  PythonQtObjectPtr p;
  PyObject* dict = NULL;
  if (PyModule_Check(_context)) {
    dict = PyModule_GetDict(_context);
  } else if (PyDict_Check(_context)) {
    dict = _context;
  }
  if (dict) {
    p.setNewRef(PyRun_String(QStringToPythonConstCharPointer(code), Py_file_input, dict, dict));
  }

  if (!p) {
    PythonQt::self()->handleError();
  }

  flushStdOut();

}


//-----------------------------------------------------------------------------

void PythonQtConsole::setCurrentFont(const QColor& color, bool bold) {

  QTextCharFormat charFormat(_defaultTextCharacterFormat);

  QFont font(charFormat.font());
  font.setBold(bold);
  charFormat.setFont(font);

  QBrush brush(charFormat.foreground());
  brush.setColor(color);
  charFormat.setForeground(brush);

  setCurrentCharFormat(charFormat);
}



//-----------------------------------------------------------------------------

int PythonQtConsole::commandPromptPosition() {

  QTextCursor textCursor(this->textCursor());
  textCursor.movePosition(QTextCursor::End);

  return textCursor.block().position() + _commandPrompt.length();
}



//-----------------------------------------------------------------------------

void PythonQtConsole::insertCompletion(const QString& completion)
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
  if (tc.selectedText()==".") {
    tc.insertText(QString(".") + completion);
  } else {
    tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    setTextCursor(tc);
  }
}

//-----------------------------------------------------------------------------
void PythonQtConsole::handleTabCompletion()
{
  QTextCursor textCursor = this->textCursor();
  int pos = textCursor.position();
  textCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  int startPos = textCursor.selectionStart();

  int offset = pos-startPos;
  QString text = textCursor.selectedText();

  QString textToComplete;
  int cur = offset;
  while (cur--) {
    QChar c = text.at(cur);
    if (c.isLetterOrNumber() || c == '.' || c == '_') {
      textToComplete.prepend(c);
    } else {
      break;
    }
  }

  QString lookup;
  QString compareText = textToComplete;
  int dot = compareText.lastIndexOf('.');
  if (dot!=-1) {
    lookup = compareText.mid(0, dot);
    compareText = compareText.mid(dot+1, offset);
  }
  if (!lookup.isEmpty() || !compareText.isEmpty()) {
    compareText = compareText.toLower();
    QStringList found;
    QStringList l = PythonQt::self()->introspection(_context, lookup, PythonQt::Anything);
    Q_FOREACH (QString n, l) {
        if (n.toLower().startsWith(compareText)) {
          found << n;
        }
      }

    if (!found.isEmpty()) {
      _completer->setCompletionPrefix(compareText);
      _completer->setCompletionMode(QCompleter::PopupCompletion);
      _completer->setModel(new QStringListModel(found, _completer));
      _completer->setCaseSensitivity(Qt::CaseInsensitive);
      QTextCursor c = this->textCursor();
      c.movePosition(QTextCursor::StartOfWord);
      QRect cr = cursorRect(c);
      cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                  + _completer->popup()->verticalScrollBar()->sizeHint().width());
      cr.translate(0,8);
      _completer->complete(cr);
    } else {
      _completer->popup()->hide();
    }
  } else {
    _completer->popup()->hide();
  }
}

void PythonQtConsole::keyPressEvent(QKeyEvent* event) {

  if (_completer && _completer->popup()->isVisible()) {
    // The following keys are forwarded by the completer to the widget
    switch (event->key()) {
      case Qt::Key_Return:
        if (!_completer->popup()->currentIndex().isValid()) {
          insertCompletion(_completer->currentCompletion());
          _completer->popup()->hide();
          event->accept();
        }
        event->ignore();
        return;
        break;
      case Qt::Key_Enter:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:

        event->ignore();
        return; // let the completer do default behavior
      default:
        break;
    }
  }
  bool        eventHandled = false;
  QTextCursor textCursor   = this->textCursor();

  int key = event->key();
  switch (key) {
    case Qt::Key_Tab:

      this->insertPlainText(QString(" ").repeated(2 - this->textCursor().positionInBlock() % 2));
      eventHandled = true;
      break;

    default:
      break;
  }

  if (eventHandled) {

    _completer->popup()->hide();
    event->accept();

  } else {

    cedar::proc::gui::CodeWidgetScope::CodeEditor::keyPressEvent(event);
    QString text = event->text();
    if (!text.isEmpty()) {
      handleTabCompletion();
    } else {
      _completer->popup()->hide();
    }
    eventHandled = true;
  }
}


//-----------------------------------------------------------------------------

void PythonQtConsole::setScene(cedar::proc::gui::Scene* pScene)
{
  mpScene = pScene;
}

void PythonQtConsole::consoleMessage(const QString & message, bool error) {

  if(!error)
  {
  cedar::aux::LogSingleton::getInstance()->message
          (
                  std::string("[CoPY] ").append(message.toStdString()),
                  "void cedar::proc::gui::PythonQtConsole::consoleMessage(const QString & text)",
                  "CoPY"
          );
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error(
              std::string("[CoPY] ").append(message.toStdString()),
              "void cedar::proc::gui::PythonQtConsole::consoleMessage(const QString & text)",
              "CoPY"
      );
  }
}
