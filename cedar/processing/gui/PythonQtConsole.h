

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

#include <boost/python.hpp>
#include "PythonQt.h"
#include "Scene.h"
#include "CoPYObject.h"
#include <QVariant>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QQueue>
#include <QWaitCondition>
#include <QMutexLocker>
#include <frameobject.h>


class QCompleter;

namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace PythonQtConsoleScope
      {
        /*class PythonWorker : public QObject
        {
        Q_OBJECT
          PythonQtObjectPtr mContext;
          cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
          cedar::proc::gui::CoPYObject *mpPy;

        public:
          PythonWorker()
          {
            //init PythonQt with mpConsole
            PythonQt::setEnableThreadSupport(true);
            PythonQt::init(PythonQt::RedirectStdOut);

            //Import CoPYObject Type To Python
            qRegisterMetaType<cedar::proc::gui::CoPYObject>("CoPYObject");
            PythonQt::self()->registerCPPClass("CoPYObject", "", "copy",
                                               PythonQtCreateObject<cedar::proc::gui::CoPYObjectWrapper>);

            //Init Wrapper of Type QObject to import in Python
            mpPyWrap = new cedar::proc::gui::CoPYObjectWrapper();
            mpPy = mpPyWrap->getO();

            mContext = PythonQt::self()->createUniqueModule();
            PyThreadState *state = PyEval_SaveThread();
            emit setup(mContext);

          };

        public slots:

          void executeCode(const QString &code)
          {
            PYTHONQT_GIL_SCOPE
            PythonQtObjectPtr p;
            PyObject *dict = NULL;
            if (PyModule_Check(mContext))
            {
              dict = PyModule_GetDict(mContext);
            } else if (PyDict_Check(mContext))
            {
              dict = mContext;
            }
            if (dict)
            {
              p.setNewRef(PyRun_String(QStringToPythonConstCharPointer(code), Py_file_input, dict, dict));
            }
            if (!p)
            {
              PythonQt::self()->handleError();
            }
          }

          void addVariable(const QString &name, const QVariant &variable)
          {
            PYTHONQT_GIL_SCOPE
            mContext.addVariable(name, variable);
          }

          void reset()
          {
            PYTHONQT_GIL_SCOPE
            mContext = PythonQt::self()->createUniqueModule();

            //reassign both cpp and python Object with getObject()-Method to be of type cedar::proc::gui::CoPYObject not cedar::proc::gui::CoPYObjectWrapper
            mContext.addObject("pyWrap", mpPyWrap);
            mContext.evalScript("from PythonQt.copy import CoPYObject\npy = pyWrap.object()\n");
          }

          void setScene(cedar::proc::gui::Scene *pScene)
          {
            mpPy->setScene(pScene);
            mpPy->setGroup(pScene->getRootGroup()->getGroup());
          };
        signals:

          void setup(PythonQtObjectPtr mContext);
        };*/

        class ThreadedPythonContext : public QThread
        {
        Q_OBJECT
        private:
          //static bool hasToStop;
          static int counter;
        public:
          ThreadedPythonContext() :
                  QThread(),
                  _running(true)
          {

            //init PythonQt with mpConsole
            PythonQt::setEnableThreadSupport(true);
            PythonQt::init(PythonQt::IgnoreSiteModule || PythonQt::RedirectStdOut);
            //PythonQt::init(PythonQt::IgnoreSiteModule|PythonQt::RedirectStdOut);

            //Import CoPYObject Type To Python
            qRegisterMetaType<cedar::proc::gui::CoPYObject>("CoPYObject");
            PythonQt::self()->registerCPPClass("CoPYObject", "", "copy",
                                               PythonQtCreateObject<cedar::proc::gui::CoPYObjectWrapper>);

            //Init Wrapper of Type QObject to import in Python
            mpPyWrap = new cedar::proc::gui::CoPYObjectWrapper();
            mpPy = mpPyWrap->getO();

            _context = PythonQt::self()->getMainModule();
            _context.addObject("pyWrap", mpPyWrap);
            _context.evalScript("from PythonQt.copy import CoPYObject\npy = pyWrap.object()\n");


            PyThreadState *state = PyEval_SaveThread();
          }

          ~ThreadedPythonContext()
          {
            _running = false;
            wait();
          }

          PythonQtObjectPtr getContext()
          {
            return _context;
          };

          void setScene(cedar::proc::gui::Scene *pScene)
          {
            mpPy->setScene(pScene);
            mpPy->setGroup(pScene->getRootGroup()->getGroup());
          };

          void setContext(const PythonQtObjectPtr &context)
          {
            _context = context;
          };

          void setHasToStop()
          {
            //hasToStop = true;
          }

          void issue(const QString &code)
          {
            _lock.lock();
            std::cout << code.toStdString() << std::endl;
            _commands.enqueue(code);
            _lock.unlock();

            _CommandQueued.wakeOne();
          }

          bool isCommandQueueEmpty()
          {
            QMutexLocker lock(&_lock);
            return _commands.isEmpty();
          }

          static int traceHook(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
          {
            //function should call for each python line execute

            if( false ) //hasToStop
            {
              QString type;
              if( what == PyTrace_CALL ) type = "PyTrace_CALL";
              else if( what == PyTrace_EXCEPTION ) type = "PyTrace_EXCEPTION";
              else if( what == PyTrace_LINE ) type = "PyTrace_LINE";
              else if( what == PyTrace_RETURN ) type = "PyTrace_RETURN";
              else if( what == PyTrace_C_CALL ) type = "PyTrace_C_CALL";
              else if( what == PyTrace_C_EXCEPTION ) type = "PyTrace_C_EXCEPTION";
              else if( what == PyTrace_C_RETURN ) type = "PyTrace_C_RETURN";

              PyErr_SetString( PyExc_Exception,
                               tr("Interruption of script at line [%1] [%2].")
                                       .arg(PyFrame_GetLineNumber(frame))
                                       .arg(type).toStdString().c_str() );
              //hasToStop = false;
              return -1;
            }

            return 0;
          };

        protected:
          QString dequeue()
          {
            QMutexLocker lock(&_lock);
            QString cmd(_commands.dequeue());

            return cmd.isEmpty() ? "\n" : cmd;
          }

          void run()
          {

            QMutex signal;

            while (_running)
            {
              // wait to be signaled ...
              signal.lock();
              _CommandQueued.wait(&signal, 1);
              signal.unlock();
              if (isCommandQueueEmpty())
              {
                continue;
              }
              while (!isCommandQueueEmpty())
              {

                PythonQtObjectPtr p;
                PyObject *dict = NULL;

                if (PyModule_Check(_context))
                {
                  dict = PyModule_GetDict(_context);
                } else if (PyDict_Check(_context))
                {
                  dict = _context;
                }

                if (dict)
                {
                  // this command blocks until the code has completed execution
                  emit python_busy(true);
                  PYTHONQT_GIL_SCOPE
                  PyEval_SetTrace(traceHook, _context.object());
                  _context.evalScript(dequeue().toLatin1().data());
                  emit python_busy(false);
                }

                // error in the kernel
                if (!p)
                {
                  PythonQt::self()->handleError();
                }
              }
            }
          }

          PythonQtObjectPtr _context;
          cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
          cedar::proc::gui::CoPYObject *mpPy;

          QMutex _lock;
          QQueue<QString> _commands;

          QWaitCondition _CommandQueued;
          bool _running;

        signals:

          void python_busy(bool);
        };
      }
    }
  }
}

//-------------------------------------------------------------------------------
//! A simple console for python scripting
class cedar::proc::gui::PythonQtConsole : public cedar::proc::gui::CodeWidgetScope::CodeEditor
{
Q_OBJECT

public:
  PythonQtConsole(QWidget *parent, Qt::WindowFlags i = 0);

  ~PythonQtConsole();

  inline static bool hasToStop = false;

public Q_SLOTS:
  //! derived key press event
  void keyPressEvent(QKeyEvent *e);

  //! output redirection
  void stdOut(const QString &s);

  //! output redirection
  void stdErr(const QString &s);

  //! execute current line
  void executeCode();

  void insertCompletion(const QString &);

  void handleSetup(PythonQtObjectPtr mContext);

  void handleTimeout();

signals:

  void execute(const QString &code);

  void addVariableToPython(const QString &name, const QVariant &variable);

  void resetPython();

  void giveScene(cedar::proc::gui::Scene *pScene);

public:
  //! output from console
  void consoleMessage(const QString &message, bool error);

  void executeCode(const QString &code);

  //! returns true if python cerr had an error
  bool hadError()
  { return _hadError; }

  void setScene(cedar::proc::gui::Scene *pScene);

  void reset();

  void addVariable(const QString &name, const QVariant &variable);

  QStringList getVariables();

protected:
  //! handle the pressing of tab
  void handleTabCompletion();

  //! Sets the current font
  void setCurrentFont(const QColor &color = QColor(0, 0, 0), bool bold = false);

  //! flush output that was not yet printed
  void flushStdOut();

private:
  //cedar::proc::gui::PythonQtConsoleScope::PythonWorker *mpPythonWorker;
  //QThread mPythonThread;
  PythonQtObjectPtr mpModule;
  //cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
  //cedar::proc::gui::CoPYObject *mpPy;
  cedar::proc::gui::PythonQtConsoleScope::ThreadedPythonContext mContext;

  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

  QString _clickedAnchor;
  QString _storageKey;
  QString _commandPrompt;

  QString _currentMultiLineCode;

  QString _stdOut;
  QString _stdErr;

  QTextCharFormat _defaultTextCharacterFormat;
  QCompleter *_completer;

  bool _hadError;
};


#endif

