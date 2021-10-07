

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

#ifdef CEDAR_USE_COPY
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
        class PythonWorker : public QObject
        {
        Q_OBJECT
          PythonQtObjectPtr mContext;
          cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
          cedar::proc::gui::CoPYObject *mpPy;

        public:

          static std::string hasToStop;

          PythonWorker(cedar::proc::gui::CoPYObject* pyObject)
          {

            //init PythonQt with mpConsole
            PythonQt::setEnableThreadSupport(true);
            PythonQt::init(PythonQt::IgnoreSiteModule|PythonQt::RedirectStdOut);

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
            QObject::connect(mpPyWrap, SIGNAL(connectSig(const QString &, const int &, const QString &, const int &)), mpPy, SLOT(connectSlots(const QString &, const int &, const QString &, const int &)),Qt::BlockingQueuedConnection);
            QObject::connect(mpPyWrap, SIGNAL(createGroupTemplateSig(const QString &, const int &, const int &, const QString&, const int &)), mpPy, SLOT(createGroupTemplate(const QString &, const int &, const int &, const QString&, const int &)),Qt::BlockingQueuedConnection);
            QObject::connect(mpPyWrap, SIGNAL(setParameterSig(const QString &, const QString &, const QVariant &)), mpPy, SLOT(setParameter(const QString &, const QString &, const QVariant &)),Qt::BlockingQueuedConnection);
            QObject::connect(mpPyWrap, SIGNAL(disconnectSig(const QString &, const int &, const QString &, const int &)), mpPy, SLOT(disconnectSlots(const QString &, const int &, const QString &, const int &)),Qt::BlockingQueuedConnection);
            QObject::connect(mpPyWrap, SIGNAL(addObjectListSig(const QString &, const QString &, const QString &)), mpPy, SLOT(addObjectList(const QString &, const QString &, const QString &)),Qt::BlockingQueuedConnection);

            mContext = PythonQt::self()->createUniqueModule();
            mContext.addObject("py", mpPyWrap);

            QObject::connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this,
                             SLOT(stdOut(const QString&)));
            QObject::connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this,
                             SLOT(stdErr(const QString&)));
            emit setup(mContext);

          };
        private:

          int static traceHook(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
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
        public slots:

          QMap<QString, QString> giveVariables();

          void stdOut(const QString &stdOut)
          {
            emit flushStdOut(stdOut);
          };

          void stdErr(const QString &stdErr)
          {
            emit flushStdErr(stdErr);
          }

          void executeCode(const QString &code)
          {
            emit pythonBusy(true);
            PyEval_SetTrace(traceHook, mContext.object());
            mContext.evalScript(code);
            emit pythonBusy(false);
          }


          void addVariable(const QString &name, const QVariant &variable)
          {
            mContext.addVariable(name, variable);
          }

          void reset()
          {
            mContext = PythonQt::self()->createUniqueModule();
            //reassign both cpp and python Object with getObject()-Method to be of type cedar::proc::gui::CoPYObject not cedar::proc::gui::CoPYObjectWrapper
            mContext.addObject("py", mpPyWrap);
          }

          void removeVariable(const QString& name)
          {
            mContext.removeVariable(name);
          }

          void setScene(cedar::proc::gui::Scene *pScene)
          {
            mpPy->setScene(pScene);
            mpPy->setGroup(pScene->getRootGroup()->getGroup());
          };
        signals:

          void flushStdOut(const QString &);

          void flushStdErr(const QString &);

          void setup(PythonQtObjectPtr mContext);

          void pythonBusy(const bool &);
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

  void handleBusyStateChange(const bool &state);


signals:

  void removeVariableFromPython(const QString&name);

  void giveVariables(QMap<QString, QString> vars);

  QMap<QString, QString> getVariablesFromWorker();

  void execute(const QString &code);

  void addVariableToPython(const QString &name, const QVariant &variable);

  void resetPython();

  void giveScene(cedar::proc::gui::Scene *pScene);

public:
  //! output from console
  void consoleMessage(const QString &message, bool error);

  void executeCode(const QString &code);

  void removeVariable(const QString& name);

  void setScene(cedar::proc::gui::Scene *pScene);

  void reset(std::string msg = "Aborted by User", bool fromOut = false);

  void addVariable(const QString &name, const QVariant &variable);

protected:
  //! handle the pressing of tab
  void handleTabCompletion();

  //! Sets the current font
  void setCurrentFont(const QColor &color = QColor(0, 0, 0), bool bold = false);

  //! flush output that was not yet printed
  void flushStdOut();

private:
  cedar::proc::gui::PythonQtConsoleScope::PythonWorker *mpPythonWorker;
  QThread mPythonThread;
  PythonQtObjectPtr mpModule;
  bool pythonBusy;
  cedar::proc::gui::Scene *mpScene;
  cedar::proc::gui::CoPYObject *mpCopyObject;

  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

  QString _clickedAnchor;
  QString _storageKey;
  QString _commandPrompt;

  QString _currentMultiLineCode;

  QString _stdOut;
  QString _stdErr;

  QTextCharFormat _defaultTextCharacterFormat;
  QCompleter *_completer;

};

#endif
#endif

