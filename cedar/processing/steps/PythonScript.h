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

    File:        PythonScript.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 01

    Description: Header file for the class PythonScript.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_PYTHON_SCRIPT_H
#define CEDAR_PROC_STEPS_PYTHON_SCRIPT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_PYTHON

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/FileParameter.h>
#include <cedar/auxiliaries/StringParameter.h>
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/BoolParameter.h>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/PythonScript.fwd.h"

//Python [cv::Mat / np] conversion includes
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

//#include <Python.h>
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <boost/algorithm/string.hpp>

#define NUMPY_IMPORT_ARRAY_RETVAL

// SYSTEM INCLUDES
#include <QGraphicsSceneContextMenuEvent>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>



/*!@brief This step implements a python code execution integration of cedar
 */
class cedar::proc::steps::PythonScript : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  Q_OBJECT

  
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PythonScript();
  
  //!@brief Constructor to set if step is looped
  PythonScript(bool);

  //!@brief Destructor
  ~PythonScript();

  //--------------------------------------------------------------------------------------------------------------------
  // public structs
  //--------------------------------------------------------------------------------------------------------------------

public:
  struct TemplateName{
      std::string name;
      bool isLooped;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void inputConnectionChanged(const std::string& inputName);
  
  void executeButtonClicked();
  
  void printFromPython(char const*);

  template<typename T>
  cv::Mat convert3DMatToFloat(cv::Mat&);

  
  std::string makeInputSlotName(const int);
  std::string makeOutputSlotName(const int);

  void exportStepAsTemplate();
  void appendJson(cedar::aux::Path, bool, std::string);

  static void importStepsFromTemplate();
  static void declareTemplate(const char*, bool);
  static std::vector<TemplateName> getTemplateNames();

public slots:
  
  void hasScriptFileChanged();
  void numberOfOutputsChanged();
  void numberOfInputsChanged();

signals:

  void errorMessageLineNumberChanged(long);
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  void compute(const cedar::proc::Arguments&);
  void executePythonScript();

  void freePythonVariables();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // public static members
  static int executionFailed;
  static QMutex mutex;
  static std::string nameOfExecutingStep;

protected:
  // none yet
private:

  cedar::aux::MatDataPtr mOutput;
  int isExecuting = 0;

  std::vector< cedar::aux::ConstMatDataPtr > mInputs;
  std::vector< cedar::aux::MatDataPtr > mOutputs;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  
  cedar::aux::StringParameterPtr _codeStringForSavingArchitecture;
protected:
  // none yet

private:

  cedar::aux::UIntParameterPtr _mNumberOfInputs;
  cedar::aux::UIntParameterPtr _mNumberOfOutputs;
  cedar::aux::BoolParameterPtr _hasScriptFile;
  cedar::aux::FileParameterPtr _scriptFile;
  cedar::aux::BoolParameterPtr _autoConvertDoubleToFloat; // advanced

};

namespace cedar::proc::steps::PythonScriptScope
{
  class ValidationMaskInputDialog : public QDialog
  {
    //--------------------------------------------------------------------------------------------------------------------
    // nested types
    //--------------------------------------------------------------------------------------------------------------------


  Q_OBJECT

  public:
    ValidationMaskInputDialog(std::vector<std::string> unacceptedStrings)
            :
            QDialog()
    {
      this->unacceptedStrings = unacceptedStrings;
    };

    QString getText(QWidget *parent, const QString &title, const QString &label,
                    QLineEdit::EchoMode echo = QLineEdit::Normal,
                    const QString &text = QString(), bool *ok = nullptr,
                    Qt::WindowFlags flags = Qt::WindowFlags(),
                    Qt::InputMethodHints inputMethodHints = Qt::ImhNone)
    {
      this->setWindowTitle(title);
      this->setInputMethodHints(inputMethodHints);

      QFormLayout form(this);

      // Add the lineEdit with its label
      edit = new QLineEdit(this);
      edit->setText(text);
      edit->setEchoMode(echo);
      form.addRow(label, edit);

      // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
      QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

      btnBox = &buttonBox;
      btnBox->button(QDialogButtonBox::Ok)->setEnabled(checkString(text.toStdString()));
      form.addRow(btnBox);

      QObject::connect(&buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
      QObject::connect(&buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

      QObject::connect(edit, SIGNAL(textChanged(
                                            const QString &)), this, SLOT(textChanged(
                                                                                  const QString &)));

      const int ret = this->exec();
      if (ok)
        *ok = !!ret;
      if (ret)
      {
        return edit->text();
      }
      else
      {
        return QString();
      }
    }

  public slots:

    void textChanged(const QString &text)
    {
      if (checkString(text.toStdString()))
      {
        edit->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");
        btnBox->button(QDialogButtonBox::Ok)->setEnabled(true);

      }
      else
      {
        edit->setStyleSheet("QLineEdit { background: rgb(255, 0, 0); }");
        btnBox->button(QDialogButtonBox::Ok)->setEnabled(false);
      }

    }

  private:

    bool checkString(std::string text)
    {
      boost::trim(text);
      for (int i = 0; i < unacceptedStrings.size(); i++)
      {
        if (!unacceptedStrings.at(i).compare(text))
        {
          return false;
        }
      }
      return true;
    }

  private:
    QLineEdit *edit;
    QDialogButtonBox *btnBox;
    std::vector<std::string> unacceptedStrings;

  };


  //!@brief This class performs the matrix conversion between python and c++
  class NDArrayConverter
  {
    //--------------------------------------------------------------------------------------------------------------------
    // constructors and destructor
    //--------------------------------------------------------------------------------------------------------------------
  public:

    NDArrayConverter();

    NDArrayConverter(cedar::proc::steps::PythonScript *);

    //--------------------------------------------------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------------------------------------------------
  public:

    cv::Mat toMat(PyObject *, int);

    PyObject *toNDArray(const cv::Mat &mat);

    //void copyTo(cv::Mat src, cv::OutputArray _dst);
    int failmsg(const char *, ...);

    //--------------------------------------------------------------------------------------------------------------------
    // private methods
    //--------------------------------------------------------------------------------------------------------------------
  private:

    const char *typenumToString(int);

    void init();

    //--------------------------------------------------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------------------------------------------------
  public:

    static int isInitialized;

  private:
    cedar::proc::steps::PythonScript *pythonScript;


  };


  class PyAllowThreads;

  class PyEnsureGIL;

  static size_t REFCOUNT_OFFSET = (size_t)&(((PyObject*)0)->ob_refcnt) +
                                  (0x12345678 != *(const size_t*)"\x78\x56\x34\x12\0\0\0\0\0")*sizeof(int);

  static inline PyObject* pyObjectFromRefcount(const int* refcount)
  {
    return (PyObject*)((size_t)refcount - REFCOUNT_OFFSET);
  }

  static inline int* refcountFromPyObject(const PyObject* obj)
  {
    return (int*)((size_t)obj + REFCOUNT_OFFSET);
  }


  class NumpyAllocator;
}



//enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };

#endif // CEDAR_USE_PYTHON

#endif // CEDAR_PROC_STEPS_PYTHON_SCRIPT_H

