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

    File:        CodeWidget.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 08

    Description: Implementation of the @em cedar::aux::gui::CodeWidget class.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/configuration.h"

#include "cedar/processing/steps/PythonScript.h"
#include "cedar/processing/gui/CodeWidget.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QTextCursor>
#include <vector>
#include <QTextEdit>
#include <QPainter>


#ifdef CEDAR_USE_PYTHON


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



cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
  keywords = QStringList() << "as" << "and" << "assert" << "break" << "class" << "continue" << "def" <<
        "del" << "elif" << "else" << "except" << "exec" << "finally" <<
        "for" << "from" << "global" << "if" << "import" << "in" <<
        "is" << "lambda" << "not" << "or" << "pass" << "print" <<
        "raise" << "return" << "try" << "while" << "yield" <<
        "None" << "True" << "False";

  operators = QStringList() << "=" <<
        // Comparison
        "==" << "!=" << "<" << "<=" << ">" << ">=" <<
        // Arithmetic
        "\\+" << "-" << "\\*" << "/" << "//" << "%" << "\\*\\*" <<
        // In-place
        "\\+=" << "-=" << "\\*=" << "/=" << "%=" <<
        // Bitwise
        "\\^" << "\\|" << "&" << "~" << ">>" << "<<";

  braces = QStringList() << "{" << "}" << "\\(" << "\\)" << "\\[" << "]";

  basicStyles.insert("keyword", getTextCharFormat("blue"));
  basicStyles.insert("operator", getTextCharFormat("red"));
  basicStyles.insert("brace", getTextCharFormat("darkGray"));
  basicStyles.insert("defclass", getTextCharFormat("black", "bold"));
  
  QTextCharFormat braceCharFormat;
  braceCharFormat.setForeground(QColor(80, 80, 80));
  basicStyles.insert("brace", braceCharFormat);
  
  basicStyles.insert("string", getTextCharFormat("magenta"));
  basicStyles.insert("string2", getTextCharFormat("darkMagenta"));
  basicStyles.insert("comment", getTextCharFormat("darkGreen", "italic"));
  basicStyles.insert("self", getTextCharFormat("black", "italic"));
  basicStyles.insert("numbers", getTextCharFormat("brown"));

  triSingleQuote.setPattern("'''");
  triDoubleQuote.setPattern("\"\"\"");

  initializeRules();
  
  
}


void cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter::initializeRules()
{
  foreach (QString currKeyword, keywords)
  {
    rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule(QString("\\b%1\\b").arg(currKeyword), 0, basicStyles.value("keyword")));
  }
  foreach (QString currOperator, operators)
  {
    rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule(QString("%1").arg(currOperator), 0, basicStyles.value("operator")));
  }
  foreach (QString currBrace, braces)
  {
    rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule(QString("%1").arg(currBrace), 0, basicStyles.value("brace")));
  }

  // 'self'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\bself\\b", 0, basicStyles.value("self")));

  // Double-quoted string, possibly containing escape sequences
  // FF: originally in python : r'"[^"\\]*(\\.[^"\\]*)*"'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"", 0, basicStyles.value("string")));
  // Single-quoted string, possibly containing escape sequences
  // FF: originally in python : r"'[^'\\]*(\\.[^'\\]*)*'"
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("'[^'\\\\]*(\\\\.[^'\\\\]*)*'", 0, basicStyles.value("string")));

  // 'def' followed by an identifier
  // FF: originally: r'\bdef\b\s*(\w+)'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\bdef\\b\\s*(\\w+)", 1, basicStyles.value("defclass")));
  //  'class' followed by an identifier
  // FF: originally: r'\bclass\b\s*(\w+)'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\bclass\\b\\s*(\\w+)", 1, basicStyles.value("defclass")));

  // From '#' until a newline
  // FF: originally: r'#[^\\n]*'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("#[^\\n]*", 0, basicStyles.value("comment")));

  // Numeric literals
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\b[+-]?[0-9]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?[0-9]+[lL]?\b'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b'
  rules.append(cedar::proc::gui::CodeWidgetScope::HighlightingRule("\\b[+-]?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b'
}


void cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter::highlightBlock(const QString &text)
{ 
  foreach (HighlightingRule currRule, rules)
  {
    int idx = currRule.pattern.indexIn(text, 0);
    while (idx >= 0)
    {
      // Get index of Nth match
      idx = currRule.pattern.pos(currRule.nth);
      int length = currRule.pattern.cap(currRule.nth).length();
      setFormat(idx, length, currRule.format);
      idx = currRule.pattern.indexIn(text, idx + length);
    }
  }
    
  setCurrentBlockState(0);

  // Do multi-line strings
  bool isInMultilne = matchMultiline(text, triSingleQuote, 1, basicStyles.value("string2"));
  if (!isInMultilne)
  isInMultilne = matchMultiline(text, triDoubleQuote, 2, basicStyles.value("string2"));
}





bool cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter::matchMultiline(const QString &text, const QRegExp &delimiter, const int inState, const QTextCharFormat &style)
{
  int start;
  int add;
  int end;
  int length = 0;

  // If inside triple-single quotes, start at 0
  if (previousBlockState() == inState) {
    start = 0;
    add = 0;
  }
  // Otherwise, look for the delimiter on this line
  else {
    start = delimiter.indexIn(text);
    // Move past this match
    add = delimiter.matchedLength();
  }

  // As long as there's a delimiter match on this line...
  while (start >= 0) {
    // Look for the ending delimiter
    end = delimiter.indexIn(text, start + add);
    // Ending delimiter on this line?
    if (end >= add) {
      length = end - start + add + delimiter.matchedLength();
      setCurrentBlockState(0);
    }
    // No; multi-line string
    else {
      setCurrentBlockState(inState);
      length = text.length() - start + add;
    }
    // Apply formatting and look for next
    setFormat(start, length, style);
    start = delimiter.indexIn(text, start + length);
  }
  // Return True if still inside a multi-line string, False otherwise
  if (currentBlockState() == inState)
    return true;
  else
    return false;
}

const QTextCharFormat cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter::getTextCharFormat(const QString &colorName, const QString &style)
{
  QTextCharFormat charFormat;
  QColor color(colorName);
  charFormat.setForeground(color);
  if (style.contains("bold", Qt::CaseInsensitive))
    charFormat.setFontWeight(QFont::Bold);
  if (style.contains("italic", Qt::CaseInsensitive))
    charFormat.setFontItalic(true);
  return charFormat;
}

int cedar::proc::gui::CodeWidget::dontMark;

cedar::proc::gui::CodeWidget::CodeWidget()
:
highlighter(nullptr)
{
  cedar::proc::gui::CodeWidget::dontMark = 0;
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);

}

cedar::proc::gui::CodeWidget::CodeWidget(QWidget* pParent)
:
QWidget(pParent),
highlighter(nullptr)
{
  cedar::proc::gui::CodeWidget::dontMark = 0;
  mMainLayout = new QVBoxLayout();
  this->setLayout(this->mMainLayout);
}

cedar::proc::gui::CodeWidget::~CodeWidget()
{
  delete mMainLayout;
}

void cedar::proc::gui::CodeWidget::setConnectable(cedar::proc::steps::PythonScriptPtr pythonScript)
{
  this->mPythonScript = pythonScript;
  refreshWidget();
}

void cedar::proc::gui::CodeWidget::refreshWidget()
{
  // reset the widget
  clearLayout();
  // Check if step not null
  if (!this->mPythonScript)
  {
    return;
  }
  // draw Headers
  this->createHeader(this->mPythonScript->getName());

  std::string codeText = this->mPythonScript->_codeStringForSavingArchitecture->getValue();

  this->fillInCodeSection(codeText);

  QObject::connect(mPythonScript.get(), SIGNAL(errorMessageLineNumberChanged(long)), this, SLOT(errorMessageLineNumberUpdated(long)));
  
  mMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void cedar::proc::gui::CodeWidget::errorMessageLineNumberUpdated(long lineno)
{
  mCodeTextField->markErrorLine(lineno);
}

void cedar::proc::gui::CodeWidget::clear()
{
  this->mPythonScript.reset();
  this->clearLayout();
}

void cedar::proc::gui::CodeWidget::clearLayout()
{
  int counter = 0;
  int counterWidget = 0;

  QLayoutItem *item;
  while ((item = mMainLayout->takeAt(0)) != nullptr)
  {
    counter= counter +1;
    QLayoutItem* widget;
    while((widget = item->layout()->takeAt(0)) != nullptr)
    {
      counterWidget = counterWidget+1;
      delete widget->widget();
    }
    counterWidget = 0;
    delete item->layout();
  }
}




void cedar::proc::gui::CodeWidget::createHeader(const std::string& name)
{  
  QHBoxLayout* step_name_layout = new QHBoxLayout();
  // Create step name.
  QLabel* step_name = new QLabel(QString(name.c_str()));
  QFont step_name_font= step_name->font();

  step_name_font.setPointSize(step_name_font.pointSize() + 2);
  step_name_font.setKerning(true);
  step_name_font.setWeight( QFont::Light );

  step_name->setFont(step_name_font);


  // imitate the format of aux::gui::Configurable
  step_name_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  step_name_layout->addWidget(step_name);

  mMainLayout->addLayout(step_name_layout);

}

void cedar::proc::gui::CodeWidget::fillInCodeSection(std::string& code)
{
  // Font
  QFont font;
  font.setFamily("Courier");
  font.setStyleHint(QFont::Monospace);
  font.setFixedPitch(true);
  font.setPointSize(10);
  
  QVBoxLayout* code_widget_layout = new QVBoxLayout();
  
  mCodeTextField = new cedar::proc::gui::CodeWidgetScope::CodeEditor();
  mCodeTextField->setPlainText(QString::fromStdString(code));
  mCodeTextField->setFont(font);
  mCodeTextField->setLineWrapMode(mCodeTextField->NoWrap);
    
  // Syntax Highlighting
  highlighter = new cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter(mCodeTextField->document());

  QObject::connect(mCodeTextField, SIGNAL(textChanged()), this, SLOT(updateCodeString()));
  
  
  code_widget_layout->addWidget(mCodeTextField);
  
  mExecuteButton = new QPushButton("Execute");
  QObject::connect(mExecuteButton, SIGNAL (clicked()), this, SLOT(executeButtonClicked()));
  code_widget_layout->addWidget(mExecuteButton);

  mMainLayout->addLayout(code_widget_layout);
  
}

void cedar::proc::gui::CodeWidget::executeButtonClicked(){
  if(mCodeTextField != nullptr)
  {
    if(this->mPythonScript)
    {
      this->mPythonScript->executeButtonClicked();
    }
  }
}

void cedar::proc::gui::CodeWidget::updateCodeString()
{
  if(mCodeTextField != nullptr)
  {
    if(this->mPythonScript)
    {
      this->mPythonScript->_codeStringForSavingArchitecture->setValue(mCodeTextField->toPlainText().toStdString());
    }
  }
}



// CodeEditor implementation

cedar::proc::gui::CodeWidgetScope::CodeEditor::CodeEditor(QWidget *parent)
:
QPlainTextEdit(parent)
{
  lineNumberArea = new LineNumberArea(this);

  cedar::proc::gui::CodeWidget::dontMark = 0;

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

cedar::proc::gui::CodeWidgetScope::CodeEditor::~CodeEditor(){
  cedar::proc::gui::CodeWidget::dontMark = 1;
}

int cedar::proc::gui::CodeWidgetScope::CodeEditor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::updateLineNumberAreaWidth(int)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
  if (dy)
    lineNumberArea->scroll(0, dy);
  else
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(180);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::markErrorLine(long lineno)
{

  if(!cedar::proc::gui::CodeWidget::dontMark && lineno > 0) {

    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
      QTextEdit::ExtraSelection selection;

      QColor lineColor = QColor(Qt::red).lighter(180);

      selection.format.setBackground(lineColor);
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);

      QTextCursor cursor = textCursor();
      cursor.movePosition(QTextCursor::Start);
      cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineno - 1);
      selection.cursor = cursor;
      selection.cursor.clearSelection();
      extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
  }
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), QColor(Qt::lightGray).lighter(120));
  QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void cedar::proc::gui::CodeWidgetScope::CodeEditor::keyPressEvent(QKeyEvent *e)
{
  QTextCursor text_cursor = this->textCursor();
  int key = e->key();

  // Replace tab with two spaces
  if(key == Qt::Key_Tab)
  {
    int amount = 2 - this->textCursor().positionInBlock() % 2;
    QString str = " ";
    this->insertPlainText(str.repeated(amount));
  }
  else
  {
    QPlainTextEdit::keyPressEvent(e);
  }
}


#endif // CEDAR_USE_PYTHON
