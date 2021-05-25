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
#include "CoPYObject.h"


#include <QMenu>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QCompleter>
#include <QStringListModel>
#include <QScrollBar>
#include <QAbstractItemView>

//-----------------------------------------------------------------------------

cedar::proc::gui::PythonQtConsole::PythonQtConsole(QWidget *parent, Qt::WindowFlags windowFlags)
        :
        cedar::proc::gui::CodeWidgetScope::CodeEditor(parent)
{
  setWindowFlags(windowFlags);

  _defaultTextCharacterFormat = currentCharFormat();
  _hadError = false;

  _completer = new QCompleter(this);
  _completer->setWidget(this);
  QObject::connect(_completer, SIGNAL(activated(const QString&)),
                   this, SLOT(insertCompletion(const QString&)));

  mpModule = mContext.getContext();

  clear();

  //FOR INITIALIZATION IN SAME THREAD

  //PythonQt::setEnableThreadSupport(true);
  //PythonQt::init(PythonQt::IgnoreSiteModule || PythonQt::RedirectStdOut);

  //Import CoPYObject Type To Python
  //qRegisterMetaType<cedar::proc::gui::CoPYObject>("CoPYObject");
  //PythonQt::self()->registerCPPClass("CoPYObject", "", "copy",PythonQtCreateObject<cedar::proc::gui::CoPYObjectWrapper>);

  //Init Wrapper of Type QObject to import in Python
  //mpPyWrap = new cedar::proc::gui::CoPYObjectWrapper();
  //mpPy = mpPyWrap->getO();
  //PythonQt::self()->moveToThread(dynamic_cast<QThread*>(&mContext));

/*
  mpModule = PythonQt::self()->createUniqueModule();
  mpModule.addObject("pyWrap", mpPyWrap);
  mpModule.evalScript("from PythonQt.copy import CoPYObject\npy = pyWrap.object()\n");

  mContext.setContext(mpModule);*/

  mContext.start();

 // PyThreadState* state = PyEval_SaveThread();


  //OLD THREAD WITH SIGNALS SLOTS
  //init thread and connect

  /* mpPythonWorker = new cedar::proc::gui::PythonQtConsoleScope::PythonWorker;
   mpPythonWorker->moveToThread(&mPythonThread);
   connect(&mPythonThread, &QThread::finished, mpPythonWorker, &QObject::deleteLater);

   //connect slots for savely accessing other thread
   connect(this, &cedar::proc::gui::PythonQtConsole::execute, mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::executeCode);
   connect(this, &cedar::proc::gui::PythonQtConsole::resetPython, mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::reset);
   connect(this, &cedar::proc::gui::PythonQtConsole::giveScene, mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::setScene);
   connect(this, &cedar::proc::gui::PythonQtConsole::addVariableToPython, mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::addVariable);
   connect(mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::setup, this, &cedar::proc::gui::PythonQtConsole::handleSetup);

   mPythonThread.start();*/

  QObject::connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
  QObject::connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdErr(const QString&)));

  insertPlainText(
          "#Controller imported as py\n#Drop steps here for creation and further instructions\npy.setParameter(\"root.Neural Field\",\"sizes\",[2,2])");
}

//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::stdOut(const QString &s)
{
  _stdOut += s;
  int idx;
  while ((idx = _stdOut.indexOf('\n')) != -1)
  {
    consoleMessage(_stdOut.left(idx), false);
    std::cout << QStringToPythonConstCharPointer(_stdOut.left(idx)) << std::endl;
    _stdOut = _stdOut.mid(idx + 1);
  }
}

void cedar::proc::gui::PythonQtConsole::stdErr(const QString &s)
{
  _hadError = true;
  _stdErr += s;
  int idx;
  while ((idx = _stdErr.indexOf('\n')) != -1)
  {
    consoleMessage(_stdErr.left(idx), true);
    std::cerr << QStringToPythonConstCharPointer(_stdErr.left(idx)) << std::endl;
    _stdErr = _stdErr.mid(idx + 1);
  }
}

void cedar::proc::gui::PythonQtConsole::flushStdOut()
{
  if (!_stdOut.isEmpty())
  {
    stdOut("\n");
  }
  if (!_stdErr.isEmpty())
  {
    stdErr("\n");
  }
}

//-----------------------------------------------------------------------------

cedar::proc::gui::PythonQtConsole::~PythonQtConsole()
{
  /*mContext.quit();
  mContext.wait();*/
}

//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::addVariable(const QString &name, const QVariant &variable)
{
  PYTHONQT_GIL_SCOPE
  //emit addVariableToPython(name, variable);
  mpModule.addVariable(name, variable);
}

void cedar::proc::gui::PythonQtConsole::executeCode()
{
  /*QThread* timerThread = new QThread(this);
  QTimer* timer = new QTimer(0); //parent must be null
  timer->setInterval(5000);
  timer->moveToThread(timerThread);
  connect(timerThread, SIGNAL(started()), timer, SLOT(start()));
  connect(timer, SIGNAL(timeout()),this, SLOT(handleTimeout()));
  connect(timer, SIGNAL(timeout()),timerThread, SLOT(quit()));
  timerThread->start();*/

  this->executeCode(this->toPlainText());
}


void cedar::proc::gui::PythonQtConsole::executeCode(const QString &code)
{
  // evaluate the code
  _stdOut = "";
  _stdErr = "";


  //emit execute(code);
  //PYTHONQT_GIL_SCOPE

  mContext.issue(code);

  //FOR EVALUATION IN SAME THREAD

 /* PYTHONQT_GIL_SCOPE
  mpModule.evalScript(code);*/
 /* PythonQtObjectPtr p;
  PyObject* dict = NULL;
  if (PyModule_Check(mpModule)) {
    dict = PyModule_GetDict(mpModule);
  } else if (PyDict_Check(mpModule)) {
    dict = mpModule;
  }
  if (dict) {
    p.setNewRef(PyRun_String(QStringToPythonConstCharPointer(code), Py_file_input, dict, dict));
  }
  if (!p) {
    PythonQt::self()->handleError();
  }*/
}


//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::setCurrentFont(const QColor &color, bool bold)
{
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

void cedar::proc::gui::PythonQtConsole::insertCompletion(const QString &completion)
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
  if (tc.selectedText() == ".")
  {
    tc.insertText(QString(".") + completion);
  } else
  {
    tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.insertText(completion);
    setTextCursor(tc);
  }
}

//-----------------------------------------------------------------------------
void cedar::proc::gui::PythonQtConsole::handleTabCompletion()
{
  QTextCursor textCursor = this->textCursor();
  int pos = textCursor.position();
  textCursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
  int startPos = textCursor.selectionStart();

  int offset = pos - startPos;
  QString text = textCursor.selectedText();
  QString textToComplete;
  int cur = offset;
  while (cur--)
  {
    QChar c = text.at(cur);
    if (c.isLetterOrNumber() || c == '.' || c == '_')
    {
      textToComplete.prepend(c);
    } else
    {
      break;
    }
  }

  QString lookup;
  QString compareText = textToComplete;
  int dot = compareText.lastIndexOf('.');
  if (dot != -1)
  {
    lookup = compareText.mid(0, dot);
    compareText = compareText.mid(dot + 1, offset);
  }
  if (!lookup.isEmpty() || !compareText.isEmpty())
  {
    compareText = compareText.toLower();
    QStringList found;
    PYTHONQT_GIL_SCOPE
    QStringList l = PythonQt::self()->introspection(mpModule, lookup, PythonQt::Anything);
    l.append({"setParameter", "copy", "create", "connect", "createGroup", "setGroup"});
    Q_FOREACH (QString n, l)
      {
        if (n.toLower().startsWith(compareText))
        {
          found << n;
        }
      }

    if (!found.isEmpty())
    {
      _completer->setCompletionPrefix(compareText);
      _completer->setCompletionMode(QCompleter::PopupCompletion);
      _completer->setModel(new QStringListModel(found, _completer));
      _completer->setCaseSensitivity(Qt::CaseInsensitive);
      QTextCursor c = this->textCursor();
      c.movePosition(QTextCursor::StartOfWord);
      QRect cr = cursorRect(c);
      cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                  + _completer->popup()->verticalScrollBar()->sizeHint().width());
      cr.translate(0, 8);
      _completer->complete(cr);
    } else
    {
      _completer->popup()->hide();
    }
  } else
  {
    _completer->popup()->hide();
  }
}

void cedar::proc::gui::PythonQtConsole::keyPressEvent(QKeyEvent *event)
{

  if (_completer && _completer->popup()->isVisible())
  {
    // The following keys are forwarded by the completer to the widget
    switch (event->key())
    {
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
  bool eventHandled = false;
  QTextCursor textCursor = this->textCursor();

  int key = event->key();
  switch (key)
  {
    case Qt::Key_Tab:

      this->insertPlainText(QString(" ").repeated(2 - this->textCursor().positionInBlock() % 2));
      eventHandled = true;
      break;
    default:
      break;
  }

  if (eventHandled)
  {
    _completer->popup()->hide();
    event->accept();

  } else
  {
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

void cedar::proc::gui::PythonQtConsole::setScene(cedar::proc::gui::Scene *pScene)
{
  //emit giveScene(pScene);
  //mpPy->setScene(pScene);
  //mpPy->setGroup(pScene->getRootGroup()->getGroup());
  mContext.setScene(pScene);
}

//todo
QStringList cedar::proc::gui::PythonQtConsole::getVariables()
{
  PYTHONQT_GIL_SCOPE
  QStringList output = PythonQt::self()->introspectObject(mpModule, PythonQt::Anything);

  for (QString o : output)
    std::cout << o.toStdString() << std::endl;

  return output;
}

void cedar::proc::gui::PythonQtConsole::reset()
{
  /*mpContext->quit();
   mpContext->wait();
   mpContext->start();*/
  //emit resetPython();
  //mpModule = PythonQt::self()->createUniqueModule();
  //mContext.setContext(mpModule);
  //reassign both cpp and python Object with getObject()-Method to be of type cedar::proc::gui::CoPYObject not cedar::proc::gui::CoPYObjectWrapper
  //mpModule.addObject("pyWrap", mpPyWrap);
  //mpModule.evalScript("from PythonQt.copy import CoPYObject\npy = pyWrap.object()\n");
}

void cedar::proc::gui::PythonQtConsole::consoleMessage(const QString &message, bool error)
{
  if (!error)
  {
    cedar::aux::LogSingleton::getInstance()->message
            (
                    message.toStdString(),
                    "void cedar::proc::gui::PythonQtConsole::consoleMessage(const QString & text)",
                    "CoPY"
            );
  } else if (!message.startsWith("Type") && !message.startsWith("File") && !message.startsWith("Traceback") &&
             !message.startsWith("Line"))
  {
    cedar::aux::LogSingleton::getInstance()->error(
            message.trimmed().toStdString(),
            "void cedar::proc::gui::PythonQtConsole::consoleMessage(const QString & text)",
            "CoPY"
    );
  }
}

void cedar::proc::gui::PythonQtConsole::handleSetup(PythonQtObjectPtr mContext)
{
  mpModule = mContext;
};

void cedar::proc::gui::PythonQtConsole::handleTimeout()
{
  std::cout << "Timeout" << std::endl;
  //mContext.quit();
  //executeCode("exit()");
  //emit resetPython();
};

