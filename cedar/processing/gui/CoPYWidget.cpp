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

    You should have received a Copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CoPYWidget.cpp

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:

    Credits:

======================================================================================================================*/
#include "cedar/configuration.h"
#ifdef CEDAR_USE_COPY
// CEDAR INCLUDES
#include "cedar/processing/gui/CoPYWidget.h"
#include <cedar/processing/Step.h>
#include <cedar/processing/gui/StepItem.h>
#include <cedar/auxiliaries/Settings.h>

#include "ElementList.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <thread>
#include <QFileDialog>
#include <fstream>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::CoPYWidget::CoPYWidget(QWidget* pParent)
:
QWidget(pParent)
{

  //create QTLayout
  QVBoxLayout *layout = new QVBoxLayout(reinterpret_cast<QWidget *>(this));
  layout->setMargin(0);

  mpConsole = new PythonQtConsole(this);

  mpVariableView = new QTableWidget(this->parentWidget());
  mpVariableView->setWindowFlags(Qt::Tool);
  mpVariableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mpVariableView->mapToParent(QPoint(0,0));

  this->reset();

  //appendHighlighter to Editor
  this->mpHighlighter = new cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter(mpConsole->document());
  mSelCounter = 0;

  //finalize widget and button
  layout->addWidget(mpConsole);
  layout->setAlignment(Qt::AlignRight | Qt::AlignTop);

  QHBoxLayout *buttons = new QHBoxLayout();
  mpExecuteButton = new QPushButton("Execute");
  QObject::connect(mpExecuteButton, SIGNAL (clicked()), this, SLOT(executeButtonClicked()));
  buttons->addWidget(mpExecuteButton);
  mpResetButton = new QPushButton("Reset");
  QObject::connect(mpResetButton, SIGNAL (clicked()), this, SLOT(resetButtonClicked()));
  buttons->addWidget(mpResetButton);
  mpAutoResetCheckbox = new QCheckBox("Auto Reset");
  buttons->addWidget(mpAutoResetCheckbox);
  mpSaveButton = new QPushButton("Save");
  QSizePolicy policy0 = mpSaveButton->sizePolicy();
  policy0.setHorizontalStretch(.5);
  mpSaveButton->setSizePolicy(policy0);
  QObject::connect(mpSaveButton, SIGNAL (clicked()), this, SLOT(saveButtonClicked()));
  buttons->addWidget(mpSaveButton);
  mpLoadButton = new QPushButton("Load");
  QObject::connect(mpLoadButton, SIGNAL (clicked()), this, SLOT(loadButtonClicked()));
  buttons->addWidget(mpLoadButton);
  mpVariablesButton = new QPushButton("Show Vars");
  QObject::connect(mpVariablesButton, SIGNAL (clicked()), this, SLOT(getVariablesButtonClicked()));
  buttons->addWidget(mpVariablesButton);

  QObject::connect(mpConsole, SIGNAL(giveVariables(QMap<QString, QString>)), this, SLOT(setVariables(QMap<QString, QString>)));

  layout->addLayout(buttons);
  setAcceptDrops(true);
}

cedar::proc::gui::CoPYWidget::~CoPYWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// Set up methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::CoPYWidget::setScene(cedar::proc::gui::Scene* pScene)
{
  mpConsole->setScene(pScene);
}

void cedar::proc::gui::CoPYWidget::importStepInformation(cedar::proc::gui::StepItem* pStep)
{
  mpConsole->appendPlainText(QString::fromStdString('"'+ pStep->getStep()->getFullPath() + '"'));
}

void cedar::proc::gui::CoPYWidget::importStepInformation(QList<QGraphicsItem*> pSteps){
  std::list<cedar::proc::gui::StepItem*> steps;
  for (auto item : pSteps)
  {
    if (auto step = dynamic_cast<cedar::proc::gui::StepItem*>(item))
    {
      steps.push_back(step);
    }
  }
  if(steps.size() > 1)
  {
    QStringList list;
    std::string sel = "selection" + std::to_string(++mSelCounter);
    for (cedar::proc::gui::StepItem *step : steps){
      list.append(QString::fromStdString(step->getStep()->getFullPath()));
      step->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
    }
    std::string outputString;
    for (QString item : list) outputString += item.toStdString() + ", ";
    mpConsole->addVariable(QString(sel.c_str()), list);
    mpConsole->consoleMessage(QString::fromUtf8(("Selected Items were imported to CoPY as '" + sel + "'").c_str()),
                            false);
    mpConsole->executeCode("print("+ QString::fromStdString(sel) + ")");
    mpConsole->appendPlainText(QString::fromUtf8(sel.c_str()));
  }
  else
  {
    this->importStepInformation(steps.back());
  }
}

void cedar::proc::gui::CoPYWidget::appendToConsole(std::string text)
{
  mpConsole->appendPlainText(QString::fromStdString(text));
}

void cedar::proc::gui::CoPYWidget::appendParameterToText(cedar::aux::ParameterPtr param, const std::string paramPath)
{
  std::string stepName = "unkown";
  std::string text = "";
  if(auto element = dynamic_cast<cedar::proc::Element *>(param->getNamedConfigurableOwner()))
  {
    stepName =  element->getFullPath();
  }
  else if(auto element = dynamic_cast<cedar::proc::Element *>(param->getOwner()->getParent()))
  {
    stepName = element->getFullPath();
  }
  std::string paramValue;
  if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<double> *>(param.get()))
  {
    paramValue = removeTrailingZeros(paramSet->getValue());
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<std::string> *>(param.get()))
  {
    paramValue = "\"" + paramSet->getValue() + "\"";
  } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<double> *>(param.get()))
  {
    std::vector<double> valVec = paramSet->getValue();
    paramValue="[";
    for (double val : valVec)
    {
      paramValue += removeTrailingZeros(val) + ",";
    }
    paramValue = paramValue.substr(0, paramValue.length() - 1) + "]";
  } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<unsigned int> *>(param.get()))
  {
    std::vector<unsigned int> valVec = paramSet->getValue();
    paramValue="[";
    for (unsigned int val : valVec)
    {
      paramValue += std::to_string(val) + ",";
    }
    paramValue = paramValue.substr(0, paramValue.length() - 1) + "]";
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<unsigned int> *>(param.get()))
  {
    paramValue = std::to_string(paramSet->getValue());
  }  else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<bool> *>(param.get()))
  {
    paramValue = std::to_string(paramSet->getValue());
  } else if (auto paramSet = dynamic_cast<cedar::aux::EnumParameter *>(param.get()))
  {
    paramValue = "\"" + paramSet->getEnumDeclaration().get(paramSet->getValue()).prettyString() + "\"";
  } else if (auto paramSet = dynamic_cast<cedar::aux::ObjectParameter *>(param.get()))
  {
    paramValue = paramSet->getTypeId();
  }
  else if (auto paramSet = dynamic_cast<cedar::aux::FileParameter *>(param.get()))
  {
    bool pathMode = paramSet -> getPathMode();
    paramValue = "[\"" + paramSet->getPath() + "\", " + std::to_string(pathMode) + "]";
  }
  if(auto objectListParameter = boost::dynamic_pointer_cast<cedar::aux::ObjectListParameter>(param))
  {
    std::vector<std::string> typelist;
    objectListParameter ->listTypes(typelist);
    text="py.addObjectList(\"" + stepName + "\",\"" + param->getName() + "\",\"" + typelist[0] +"\")";
  }
  if(text == "") text = "py.setParameter(\""+ stepName + "\", \"" + paramPath +"\", " + paramValue + ")\n";
  this->appendToConsole(text);
}

void cedar::proc::gui::CoPYWidget::reset()
{
  mpConsole -> reset();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::proc::gui::CoPYWidget::removeTrailingZeros(const double& num)
{
  std::string str = std::to_string(num);
  // Remove trailing zeroes
  str = str.substr(0, str.find_last_not_of('0')+1);
  // If the decimal point is now the last character, remove that as well
  if(str.find(',')) str.replace(str.find(","), 1, ".");
  if(str.find('.') == str.size()-1)
  {
    str = str.substr(0, str.size()-1);
  }
  return str;
}

//!@brief main execute method for executing CoPY
void cedar::proc::gui::CoPYWidget::executeCode()
{
  mpConsole->executeCode();
  if(mpAutoResetCheckbox->isChecked()){
    this->reset();
  }
}

std::string cedar::proc::gui::CoPYWidget::getStepInfo(cedar::proc::gui::StepItem* pStep){

  return pStep->getStep()->getFullPath();
}

void cedar::proc::gui::CoPYWidget::dragEnterEvent(QDragEnterEvent *p_event)
{
  p_event->acceptProposedAction();
}

void cedar::proc::gui::CoPYWidget::dragMoveEvent(QDragMoveEvent *p_event)
{
  p_event->acceptProposedAction();
}

void cedar::proc::gui::CoPYWidget::dropEvent(QDropEvent *pEvent)
{
  auto declaration = cedar::proc::gui::ElementList::declarationFromDrop(pEvent);

  if (auto elem_declaration = dynamic_cast<const cedar::proc::ElementDeclaration *>(declaration))
  {
    mpConsole->insertPlainText(QString(QString("py.create('") + QString(elem_declaration->getClassName().c_str()) +  QString("', 0, 0, \"root\", 1)")));
  } else if (auto group_declaration = dynamic_cast<const cedar::proc::GroupDeclaration *>(declaration))
  {
    mpConsole->insertPlainText(QString(QString("py.createGroupTemplate('") + QString(group_declaration->getClassName().c_str()) +  QString("', 0, 0, \"root\", 1)")));
  }
  pEvent->acceptProposedAction();
}

//SLOTS

void cedar::proc::gui::CoPYWidget::executeButtonClicked()
{
  this->executeCode();
}

void cedar::proc::gui::CoPYWidget::resetButtonClicked()
{
  this->reset();
}

void cedar::proc::gui::CoPYWidget::getVariablesButtonClicked()
{
  mpVariableView->show();
}

bool cedar::proc::gui::CoPYWidget::saveButtonClicked()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to save", // caption
                                              last_dir->getValue().absolutePath(), // initial directory;
                                              "pythonScript (*.py)", // filter(s), separated by ';;'
                                              0,
          // js: Workaround for freezing file dialogs in QT5 (?)
                                              QFileDialog::DontUseNativeDialog
  );
  if (file.isEmpty())
  {
    return false;
  }
  if (!file.endsWith(".py"))
  {
    file += ".py";
  }
  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());
  cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(file.toStdString());
  std::ofstream outfile;
  outfile.open(file.toStdString());
  outfile << mpConsole->toPlainText().toStdString();
  outfile.close();
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  last_dir->setValue(path);
  return true;
}
void cedar::proc::gui::CoPYWidget::setVariables(QMap<QString, QString> vars)
{
  QList<QStandardItem*> keys;
  QList<QStandardItem*> values;
  mpVariableView->setRowCount(vars.count());
  mpVariableView->setColumnCount(3);
  mpVariableView->setColumnWidth(2, 10);
  mpVariableView->setGridStyle(Qt::NoPen);
  mpVariableView->clear();
  QStringList header;
  header.append(QString("Variable"));
  header.append(QString("Value"));
  header.append(QString(""));
  mpVariableView->setHorizontalHeaderLabels(header);

  int i = 0;
  for(QString var : vars.keys())
  {
    mpVariableView->setItem(i, 0, new QTableWidgetItem(var));
    mpVariableView->setItem(i, 1, new QTableWidgetItem(vars.value(var)));
    QPushButton* delBtn = new QPushButton("X");
    delBtn->setStyleSheet("font: bold;height: 10px;width: 10px;");
    delBtn->setParent(mpVariableView);
    delBtn->setVisible(true);
    mpVariableView->setCellWidget(i, 2, delBtn);
    connect(delBtn, &QPushButton::clicked, [this, var]() {
      mpConsole->removeVariable(var);
    });
    i++;
  }
}


void cedar::proc::gui::CoPYWidget::loadButtonClicked()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QFileDialog filedialog(this, "Select which file to load");
  filedialog.setNameFilter( tr("pythonScript (*.py)"));
  filedialog.setDirectory( last_dir->getValue().absolutePath()); // initial directory
  filedialog.setFilter( QDir::AllDirs | QDir::Files
                        | QDir::NoDot
                        | QDir::Hidden );
  // see hidden files to see the backup files
#ifdef CEDAR_USE_QT5
  filedialog.setOptions( QFileDialog::DontUseNativeDialog );
  // js: Workaround for freezing file dialogs in QT5 (?)
#endif
  QString file;
  if (filedialog.exec())
  {
    QStringList filelist;
    filelist = filedialog.selectedFiles();
    file= filelist.first();
  }
  if (!file.isEmpty())
  {
    std::ifstream infile(file.toStdString(), std::ios_base::binary);
    if (infile.fail())
    {
     return mpConsole->consoleMessage(QString::fromStdString("Failed to open file " + file.toStdString()), true);
    }
    std::stringstream buffer;
    buffer << infile.rdbuf();
    mpConsole->clear();
    mpConsole->appendPlainText(QString::fromStdString(buffer.str()));
  }
}
#endif