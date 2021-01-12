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

    File:        CodeWidget.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 08

    Description: Implementation of the @em cedar::aux::gui::CodeWidget class.

    Credits:

======================================================================================================================*/



#ifndef CEDAR_PROC_GUI_CODE_WIDGET_H
#define CEDAR_PROC_GUI_CODE_WIDGET_H

// CEDAR INCLUDES
#include "cedar/configuration.h"
#include "gui/PythonQtScriptingConsole.h"
// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/gui/CodeWidget.fwd.h"
#include "cedar/processing/steps/PythonScript.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsAnchorLayout>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QSyntaxHighlighter>


#ifdef CEDAR_USE_PYTHON


/*!@brief Syntax Highlighter for QTextEdit to highlight Python code
  */


/*
$Id: PythonSyntaxHighlighter.cpp 167 2013-11-03 17:01:22Z oliver $
This is a C++ port of the following PyQt example
http://diotavelli.net/PyQtWiki/Python%20syntax%20highlighting
C++ port by Frankie Simon (www.kickdrive.de, www.fuh-edv.de)

The following free software license applies for this file ("X11 license"): 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace CodeWidgetScope
      {
        //! Container to describe a highlighting rule. Based on a regular expression, a relevant match # and the format.
        class HighlightingRule
        {
        public:
          HighlightingRule(const QString &patternStr, int n, const QTextCharFormat &matchingFormat)
          {
            originalRuleStr = patternStr;
            pattern = QRegExp(patternStr);
            nth = n;
            format = matchingFormat;
          }

          QString originalRuleStr;
          QRegExp pattern;
          int nth;
          QTextCharFormat format;
        };

        //! Implementation of highlighting for Python code.
        class PythonSyntaxHighlighter : public QSyntaxHighlighter
        {
          Q_OBJECT

          public:
            PythonSyntaxHighlighter(QTextDocument *parent = 0);

          protected:
             void highlightBlock(const QString &text);

          private:
            QStringList keywords;
            QStringList operators;
            QStringList braces;

            QHash<QString, QTextCharFormat> basicStyles;

            void initializeRules();

            //! Highlights multi-line strings, returns true if after processing we are still within the multi-line section.
            bool matchMultiline(const QString &text, const QRegExp &delimiter, const int inState, const QTextCharFormat &style);
            const QTextCharFormat getTextCharFormat(const QString &colorName, const QString &style = QString());

            QList<HighlightingRule> rules;
            QRegExp triSingleQuote;
            QRegExp triDoubleQuote;
        };



        //!@brief Implementation of QPlainTextEdit with Code-Editor features (like Syntaxhighlighting, line marking, line numbers etc.)
      class CodeEditor : public QPlainTextEdit
        {
            Q_OBJECT

        public:
            CodeEditor(QWidget *parent = 0);
            ~CodeEditor();

            void lineNumberAreaPaintEvent(QPaintEvent *event);
            int lineNumberAreaWidth();
            void markErrorLine(long);

        protected:
            void resizeEvent(QResizeEvent *event);
            void keyPressEvent(QKeyEvent *event) override;

        private slots:
            void updateLineNumberAreaWidth(int newBlockCount);
            void highlightCurrentLine();
            void updateLineNumberArea(const QRect &, int);

        public:

        private:
            QWidget *lineNumberArea;
        };


        /*!@brief Widget to paint line numbers in the code section
          */
        class LineNumberArea : public QWidget
        {
        public:
            LineNumberArea(CodeEditor *editor) : QWidget(editor) {
                codeEditor = editor;
            }

            QSize sizeHint() const {
                return QSize(codeEditor->lineNumberAreaWidth(), 0);
            }

        protected:
            void paintEvent(QPaintEvent *event) {
                codeEditor->lineNumberAreaPaintEvent(event);
            }

        private:
            CodeEditor *codeEditor;
        };


      }
    }
  }
}

/*!@brief GUI representation for the code section.
  */
class cedar::proc::gui::CodeWidget
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
  CodeWidget();

  //!@brief The public constructor.
  CodeWidget(QWidget* pParent);

  //!@brief The public destructor.
  ~CodeWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the obtained step and recreating the widget to set the record parameters.
  void setConnectable(cedar::proc::steps::PythonScriptPtr pythonScript);

  void clear();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the GUI.
  void refreshWidget();

  //!@brief Create the headers for the Widget.
  void createHeader(const std::string& name);

  //!@brief Write the Code into the text field!
  void fillInCodeSection(std::string& code/*, std::string& error*/);

  //!@brief Resets the widget and its GUI elements.
  void clearLayout();

private slots:
  void updateCodeString();
  //void errorMessageUpdated();
  void errorMessageLineNumberUpdated(long);
  void executeButtonClicked();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
    static int dontMark;
private:
  //!@brief The step currently displayed.
  cedar::proc::steps::PythonScriptPtr mPythonScript;

  //!@brief The layout for this widget.
  QVBoxLayout* mpMainLayout;

  cedar::proc::gui::CodeWidgetScope::CodeEditor* mpCodeTextField;
  //QLabel* mErrorMessageLabel;
  QPushButton* mpExecuteButton;

  cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter *mpHighlighter;
};


#else // CEDAR_USE_PYTHON

class cedar::proc::gui::CodeWidget
:
public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  inline void clear() 
  {
  }
};

#endif // CEDAR_USE_PYTHON

#endif // CEDAR_PROC_GUI_CODE_WIDGET_H


