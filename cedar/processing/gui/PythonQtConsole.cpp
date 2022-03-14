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

    File:        PythonQtConsole.cpp

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:
    Credits: Florian Link - mevis.de
========================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/PythonQtConsole.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/CoPYObject.h"
#include "cedar/processing/gui/CodeWidget.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/undoRedo/UndoStack.h"

// SYSTEM INCLUDES

#include <QMenu>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QCompleter>
#include <QStringListModel>
#include <QScrollBar>
#include <QAbstractItemView>

//-----------------------------------------------------------------------------
#ifdef CEDAR_USE_COPY

//PythonQtConsole
cedar::proc::gui::PythonQtConsole::PythonQtConsole(QWidget *parent) : cedar::proc::gui::CodeWidgetScope::CodeEditor(parent)
{

  mDefaultTextCharacterFormat = currentCharFormat();

  mpCompleter = new QCompleter(this);
  mpCompleter->setWidget(this);
  QObject::connect(mpCompleter, SIGNAL(activated(const QString&)),
                   this, SLOT(insertCompletion(const QString&)));

  clear();

  //init thread and connect

  mpCopyObject = new cedar::proc::gui::CoPYObject(this);
  mpPythonWorker = new cedar::proc::gui::PythonQtConsoleScope::PythonWorker(mpCopyObject);
  mpPythonWorker->moveToThread(&mPythonThread);

  connect(&mPythonThread, &QThread::finished, mpPythonWorker, &QObject::deleteLater);
  //connect slots for savely accessing other thread
  connect(this, &cedar::proc::gui::PythonQtConsole::execute, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::executeCode);
  connect(this, &cedar::proc::gui::PythonQtConsole::getVariablesFromWorker, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::giveVariables, Qt::BlockingQueuedConnection);
  connect(this, &cedar::proc::gui::PythonQtConsole::removeVariableFromPython, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::removeVariable, Qt::BlockingQueuedConnection);
  connect(this, &cedar::proc::gui::PythonQtConsole::resetPython, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::reset);
  connect(this, &cedar::proc::gui::PythonQtConsole::giveScene, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::setScene);
  connect(this, &cedar::proc::gui::PythonQtConsole::addVariableToPython, mpPythonWorker,
          &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::addVariable);
  connect(mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::setup, this,
          &cedar::proc::gui::PythonQtConsole::handleSetup);
  connect(mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::pythonBusy, this,
          &cedar::proc::gui::PythonQtConsole::handleBusyStateChange);
  QObject::connect(mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::flushStdOut, this, &cedar::proc::gui::PythonQtConsole::stdOut, Qt::BlockingQueuedConnection);
  QObject::connect(mpPythonWorker, &cedar::proc::gui::PythonQtConsoleScope::PythonWorker::flushStdErr, this, &cedar::proc::gui::PythonQtConsole::stdErr, Qt::BlockingQueuedConnection);
  mPythonThread.start();

  insertPlainText(
          "#Controller imported as py\n#Drop steps here for creation and further instructions\ni = 0\nprint(i)\ni+=1");
  emit giveVariables(emit getVariablesFromWorker());
}

//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::stdOut(const QString &s)
{
  mStdOut += s;
  int idx;
  while ((idx = mStdOut.indexOf('\n')) != -1)
  {

    consoleMessage(mStdOut.left(idx), false);
    std::cout << QStringToPythonConstCharPointer(mStdOut.left(idx)) << std::endl;
    mStdOut = mStdOut.mid(idx + 1);
  }
}

void cedar::proc::gui::PythonQtConsole::stdErr(const QString &s)
{
  mStdErr += s;
  int idx;
  while ((idx = mStdErr.indexOf('\n')) != -1)
  {
    consoleMessage(mStdErr.left(idx), true);
    std::cerr << QStringToPythonConstCharPointer(mStdErr.left(idx)) << std::endl;
    mStdErr = mStdErr.mid(idx + 1);
  }
}

//-----------------------------------------------------------------------------

cedar::proc::gui::PythonQtConsole::~PythonQtConsole()
{
  mPythonThread.quit();
  mPythonThread.wait();
}

//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::addVariable(const QString &name, const QVariant &variable)
{
  emit addVariableToPython(name, variable);
}

void cedar::proc::gui::PythonQtConsole::removeVariable(const QString& name)
{
  emit removeVariableFromPython(name);
  emit giveVariables(emit getVariablesFromWorker());
}

void cedar::proc::gui::PythonQtConsole::executeCode()
{
  this->executeCode(this->toPlainText());
}


void cedar::proc::gui::PythonQtConsole::executeCode(const QString &code)
{
  PythonQt::init(PythonQt::PythonAlreadyInitialized|PythonQt::RedirectStdOut);
  // evaluate the code
  mStdOut = "";
  mStdErr = "";
  emit execute(code);
}


//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::setCurrentFont(const QColor &color, bool bold)
{
  QTextCharFormat charFormat(mDefaultTextCharacterFormat);

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
    //PYTHONQT_GIL_SCOPE

    QMap<QString, QString> map = emit getVariablesFromWorker();
    //QStringList l = PythonQt::self()->introspection(mpModule, lookup, PythonQt::Anything);
    //QStringList l = PythonQt::self()->introspectObject(cedar::proc::gui::PythonQtConsoleScope::PythonWorker::m mpModule, PythonQt::Anything);
    QStringList l = map.keys();
    l.append({"setParameter", "copyAllParameters", "create", "connect", "createGroupTemplate"});

    QList<QGraphicsItem*> itemsList = mpScene->items();
    for(QGraphicsItem* item : itemsList)
    {
      if(auto step = dynamic_cast<cedar::proc::gui::StepItem*>(item))
      {
        l.append(QString::fromStdString(step->getStep()->getFullPath()));
      }
    }

    Q_FOREACH (QString n, l)
      {
        if (n.toLower().startsWith(compareText))
        {
          found << n;
        }
      }

    if (!found.isEmpty())
    {
      mpCompleter->setCompletionPrefix(compareText);
      mpCompleter->setCompletionMode(QCompleter::PopupCompletion);
      mpCompleter->setModel(new QStringListModel(found, mpCompleter));
      mpCompleter->setCaseSensitivity(Qt::CaseInsensitive);
      QTextCursor c = this->textCursor();
      c.movePosition(QTextCursor::StartOfWord);
      QRect cr = cursorRect(c);
      cr.setWidth(mpCompleter->popup()->sizeHintForColumn(0)
                  + mpCompleter->popup()->verticalScrollBar()->sizeHint().width());
      cr.translate(0, 8);
      mpCompleter->complete(cr);
    } else
    {
      mpCompleter->popup()->hide();
    }
  } else
  {
    mpCompleter->popup()->hide();
  }
}

void cedar::proc::gui::PythonQtConsole::keyPressEvent(QKeyEvent *event)
{

  if (mpCompleter && mpCompleter->popup()->isVisible())
  {
    // The following keys are forwarded by the completer to the widget
    switch (event->key())
    {
      case Qt::Key_Return:
        if (!mpCompleter->popup()->currentIndex().isValid())
        {
          insertCompletion(mpCompleter->currentCompletion());
          mpCompleter->popup()->hide();
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
    mpCompleter->popup()->hide();
    event->accept();

  } else
  {
    cedar::proc::gui::CodeWidgetScope::CodeEditor::keyPressEvent(event);
    QString text = event->text();
    if (!text.isEmpty())
    {
      handleTabCompletion();
    } else
    {
      mpCompleter->popup()->hide();
    }
    eventHandled = true;
  }
}


//-----------------------------------------------------------------------------

void cedar::proc::gui::PythonQtConsole::setScene(cedar::proc::gui::Scene *pScene)
{
  mpScene = pScene;
  emit giveScene(pScene);
}

std::string cedar::proc::gui::PythonQtConsoleScope::PythonWorker::hasToStop;
void cedar::proc::gui::PythonQtConsole::reset(std::string msg, bool fromOut)
{
  if(pythonBusy)
  {
    cedar::proc::gui::PythonQtConsoleScope::PythonWorker::hasToStop = msg;
  }
  if(!pythonBusy && !fromOut)
  {
    emit resetPython();
    emit giveVariables(emit getVariablesFromWorker());
  }
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
  emit giveVariables(emit getVariablesFromWorker());
};

void cedar::proc::gui::PythonQtConsole::handleBusyStateChange(const bool& state)
{
  pythonBusy = state;
  if(pythonBusy)
  {
    cedar::proc::gui::Ide::pUndoStack->beginMacro("Python Script");
  }
  else
  {
    cedar::proc::gui::Ide::pUndoStack->endMacro();
    emit giveVariables(emit getVariablesFromWorker());
  }
}


QMap<QString, QString> cedar::proc::gui::PythonQtConsoleScope::PythonWorker::giveVariables()
{
  QMap<QString, QString> map;
  QStringList vars = PythonQt::self()->introspectObject(mContext, PythonQt::Variable);
  for (QString var: vars)
  {
    if(!var.startsWith("__") && var != "py")
    {
      QVariant val = mContext.getVariable(var);
      if (val.canConvert<QString>())
      {
        map.insert(var, val.toString());
        continue;
      }
      if (val.canConvert<QStringList>())
      {
        QString out = "[";
        for (QString oneVal : val.toStringList())
        {
          out.append(oneVal);
          out.append(", ");
        }
        out.append("]");
        map.insert(var, out);
        continue;
      }
      map.insert(var, val.typeName());

    }
  }
  QStringList funcs = PythonQt::self()->introspectObject(mContext, PythonQt::Function);
  for (QString func : funcs)
  {
    map.insert(func, QString("Function"));
  }
  QStringList classes = PythonQt::self()->introspectObject(mContext, PythonQt::Class);
  for (QString cls : classes)
  {
    map.insert(cls, QString("Class"));
  }
  return map;
}

// PythonQTWorker


cedar::proc::gui::PythonQtConsoleScope::PythonWorker::PythonWorker(cedar::proc::gui::CoPYObject* pyObject)
{
  //init PythonQt with mpConsole
  PythonQt::setEnableThreadSupport(true);
  #ifdef CEDAR_USE_PYTHONSTEP
  PythonQt::init(PythonQt::RedirectStdOut | PythonQt::PythonAlreadyInitialized);
  #else //CEDAR_USE_PYTHONSTEP
  PythonQt::init(PythonQt::RedirectStdOut);
  #endif //CEDAR_USE_PYTHONSTEP

  //PythonQt::overwriteSysPath(QStringList(QString::fromStdString("/home/fred/repos/cedar/lib/cpython")));

  //Import CoPYObject Type To Python
  //qRegisterMetaType<cedar::proc::gui::CoPYObject>("CoPYObject");
  //qRegisterMetaType<cedar::proc::gui::CoPYObject>;
  PythonQt::self()->registerCPPClass("CoPYObject", "", "copy",
  PythonQtCreateObject<cedar::proc::gui::CoPYObjectWrapper>);
  //Init Wrapper of Type QObject to import in Python
  mpPyWrap = new cedar::proc::gui::CoPYObjectWrapper();
  mpPy = pyObject;

  QObject::connect(mpPyWrap, SIGNAL(createSig(const QString &, const int &, const int &, const QString&, const int &)), mpPy, SLOT(createElem(const QString &, const int &, const int &, const QString&, const int &)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(copySig(const QString &, const QString &)), mpPy, SLOT(copyTo(const QString &, const QString &)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(connectSig(const QString &, const QVariant &, const QString &, const QVariant &, const bool &)), mpPy, SLOT(connectSlots(const QString &, const QVariant &, const QString &, const QVariant &, const bool&)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(createGroupTemplateSig(const QString &, const int &, const int &, const QString&, const int &)), mpPy, SLOT(createGroupTemplate(const QString &, const int &, const int &, const QString&, const int &)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(setParameterSig(const QString &, const QString &, const QVariant &)), mpPy, SLOT(setParameter(const QString &, const QString &, const QVariant &)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(addObjectListSig(const QString &, const QString &, const QString &)), mpPy, SLOT(addObjectList(const QString &, const QString &, const QString &)),Qt::BlockingQueuedConnection);
  QObject::connect(mpPyWrap, SIGNAL(getElementsByGroupSig(const QString &)), mpPy, SLOT(getElementsByGroup(const QString &)),Qt::BlockingQueuedConnection);

  mContext = PythonQt::self()->createUniqueModule();
  mContext.addObject("py", mpPyWrap);

  _module = PyThreadState_Get();
  QObject::connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this,
  SLOT(stdOut(const QString&)));
  QObject::connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this,
  SLOT(stdErr(const QString&)));
  emit setup(mContext);

};

cedar::proc::gui::PythonQtConsoleScope::PythonWorker::~PythonWorker()
{

}

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::stdOut(const QString &stdOut)
{
  emit flushStdOut(stdOut);
};

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::stdErr(const QString &stdErr)
{
  emit flushStdErr(stdErr);
}

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::executeCode(const QString &code)
{
  //Get Thread back for Connection
  PyThreadState_Swap(_module);

  emit pythonBusy(true);
  PyEval_SetTrace(traceHook, mContext.object());
  mContext.evalScript(code);
  emit pythonBusy(false);
}


void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::addVariable(const QString &name, const QVariant &variable)
{
  mContext.addVariable(name, variable);
}

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::reset()
{
  mContext = PythonQt::self()->createUniqueModule();
  //reassign both cpp and python Object with getObject()-Method to be of type cedar::proc::gui::CoPYObject not cedar::proc::gui::CoPYObjectWrapper
  mContext.addObject("py", mpPyWrap);
}

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::removeVariable(const QString& name)
{
  mContext.removeVariable(name);
}

void cedar::proc::gui::PythonQtConsoleScope::PythonWorker::setScene(cedar::proc::gui::Scene *pScene)
{
  mpPy->setScene(pScene);

  mpPy->setGroup(pScene->getRootGroup()->getGroup());
};

int cedar::proc::gui::PythonQtConsoleScope::PythonWorker::traceHook(PyObject *, PyFrameObject *, int , PyObject *)
{
  if (hasToStop != "")
  {
  PyErr_SetString(PyExc_Exception,
          hasToStop.c_str());
  hasToStop = "";
  return -1;
  }
  return 0;
};

#endif