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

    File:        PythonQtConsole.h

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:
    Credits: Florian Link - mevis.de
========================================================================================================================*/
#ifndef CEDAR_PROC_GUI_PYTHONQTCONSOLE_H
#define CEDAR_PROC_GUI_PYTHONQTCONSOLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_COPY

// FORWARD DECLARATIONS
#include "cedar/processing/gui/PythonQtConsole.fwd.h"
#include "cedar/auxiliaries/Log.fwd.h"
#include "cedar/auxiliaries/Singleton.fwd.h"
#include "cedar/processing/gui/CoPYObject.fwd.h"
#include "cedar/processing/GroupDeclaration.fwd.h"
#include "cedar/processing/ElementDeclaration.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"

// CEDAR INCLUDES

#include "cedar/processing/gui/CodeWidget.h"

// SYSTEM INCLUDES
#include "PythonQt.h"
#include <QCompleter>
#include <QGraphicsScene>
#include <QMutexLocker>
#include <QMutexLocker>
#include <QQueue>
#include <QTextEdit>
#include <QTextEdit>
#include <QThread>
#include <QVariant>
#include <QVariant>
#include <QWaitCondition>
#include <boost/python.hpp>
#include <frameobject.h>

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

          public:
            PythonWorker(cedar::proc::gui::CoPYObject* pyObject);

            ~PythonWorker();
          private:
            int static traceHook(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

          public slots:
            QMap<QString, QString> giveVariables();
            void stdOut(const QString &stdOut);
            void stdErr(const QString &stdErr);
            void executeCode(const QString &code);
            void addVariable(const QString &name, const QVariant &variable);
            void reset();
            void removeVariable(const QString& name);
            void setScene(cedar::proc::gui::Scene *pScene);

          signals:
            void flushStdOut(const QString &);
            void flushStdErr(const QString &);
            void setup(PythonQtObjectPtr mContext);
            void pythonBusy(const bool &);

          public:
            static std::string hasToStop;
            static PyThreadState* threadState;
          private:
            PythonQtObjectPtr mContext;
            cedar::proc::gui::CoPYObjectWrapper *mpPyWrap;
            cedar::proc::gui::CoPYObject *mpPy;
        };
      }
    }
  }
}

//-------------------------------------------------------------------------------
//! A simple console for python scripting
class cedar::proc::gui::PythonQtConsole
:
public cedar::proc::gui::CodeWidgetScope::CodeEditor
{
  Q_OBJECT

  public:
    PythonQtConsole(QWidget *parent);
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

    QString mStdOut;
    QString mStdErr;
    QTextCharFormat mDefaultTextCharacterFormat;
    QCompleter *mpCompleter;
};

#endif
#endif

