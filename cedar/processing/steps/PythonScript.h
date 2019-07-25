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

#ifdef PYTHONLIBS_FOUND

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
#include <Python.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#define NUMPY_IMPORT_ARRAY_RETVAL

// SYSTEM INCLUDES
#include <QGraphicsSceneContextMenuEvent>
#include <QContextMenuEvent>



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

public slots:
  
  void hasScriptFileChanged();
  void numberOfOutputsChanged();
  void numberOfInputsChanged();
  void exportStepAsTemplate();
  void importStepFromTemplate();
  
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

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // public static members
  static int executionFailed;
  static std::vector<PyThreadState*> threadStates;
  static QMutex mutex;
  static std::string nameOfExecutingStep;

protected:
  // none yet
private:

  cedar::aux::MatDataPtr mOutput;

  int threadID;
  
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
  cedar::aux::BoolParameterPtr _autoConvertDoubleToFloat;
  cedar::aux::FileParameterPtr _scriptFile;


};


//!@brief This class performs the matrix conversion between python and c++
class NDArrayConverter
{
    //--------------------------------------------------------------------------------------------------------------------
    // constructors and destructor
    //--------------------------------------------------------------------------------------------------------------------
public:

    NDArrayConverter();
    NDArrayConverter(cedar::proc::steps::PythonScript*);

    //--------------------------------------------------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------------------------------------------------
public:

    cv::Mat toMat(PyObject*, int);

    PyObject* toNDArray(const cv::Mat& mat);
    //void copyTo(cv::Mat src, cv::OutputArray _dst);
    int failmsg(const char*, ...);

    //--------------------------------------------------------------------------------------------------------------------
    // private methods
    //--------------------------------------------------------------------------------------------------------------------
private:

    const char * typenumToString(int);
    void init();

    //--------------------------------------------------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------------------------------------------------
public:

    static int isInitialized;

private:
    cedar::proc::steps::PythonScript* pythonScript;


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

//enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };

#endif

#endif // CEDAR_PROC_STEPS_PYTHON_SCRIPT_H

