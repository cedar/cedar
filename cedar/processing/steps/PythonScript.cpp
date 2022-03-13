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

    File:        PythonScript.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2019 04 01

    Description: Source file for the class PythonScript.

    Credits:

======================================================================================================================*/

/*
The MIT License (MIT)

Copyright (c) 2014 Yati Sagade

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_PYTHONSTEP
// CLASS HEADER
#include "cedar/processing/steps/PythonScript.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/auxiliaries//annotation/ColorSpace.h"

#include "cedar/processing/GroupDeclarationManager.h"


// SYSTEM INCLUDES
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "numpy/ndarrayobject.h"

#include <QReadWriteLock>
#include <QRegExpValidator>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::PythonScript>
      (
        "Programming",
        "cedar.processing.steps.PythonScript"
      )
    );
    declaration->setIconPath(":/steps/python_script.svg");
    declaration->setDescription
    (
      "In-Runtime Python Script-editor to develop new steps from scratch."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

// static member declaration
int cedar::proc::steps::PythonScript::executionFailed = 0;
QMutex cedar::proc::steps::PythonScript::mutex;
std::string cedar::proc::steps::PythonScript::nameOfExecutingStep = "";

int cedar::proc::steps::PythonScriptScope::NDArrayConverter::failmsg(const char *fmt, ...)
{
  char str[1000];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(str, sizeof(str), fmt, ap);
  va_end(ap);

  PyErr_SetString(PyExc_TypeError, str);

  cedar::aux::LogSingleton::getInstance()->error
          (
                  str,
                  "void cedar::proc::steps::PythonScript::executePythonScript()",
                  (this->pythonScript != nullptr) ? this->pythonScript->getName():"Python Script"
          );
  cedar::proc::steps::PythonScript::executionFailed = 1;

  return 0;
}

class cedar::proc::steps::PythonScriptScope::PyAllowThreads
{
public:
    PyAllowThreads() : _state(PyEval_SaveThread()) {}
    ~PyAllowThreads()
    {
        PyEval_RestoreThread(_state);
    }
private:
    PyThreadState* _state;
};

class cedar::proc::steps::PythonScriptScope::PyEnsureGIL
{
public:
    PyEnsureGIL() : _state(PyGILState_Ensure()) 
    {
    }
    ~PyEnsureGIL()
    {
      PyGILState_Release(_state);
    }
private:
    PyGILState_STATE _state;
};
#if CV_MAJOR_VERSION < 3
class NumpyAllocator : public cv::MatAllocator
{
public:

    NumpyAllocator() {  }
    ~NumpyAllocator() {}

    void allocate(int dims, const int* sizes, int type, int*& refcount,
                  uchar*& datastart, uchar*& data, size_t* step)
    {
      //PyEnsureGIL gil;
      int depth = CV_MAT_DEPTH(type);
      int cn = CV_MAT_CN(type);
      const int f = (int)(sizeof(size_t)/8);
      int typenum = depth == CV_8U ? NPY_UBYTE : depth == CV_8S ? NPY_BYTE :
                                                 depth == CV_16U ? NPY_USHORT : depth == CV_16S ? NPY_SHORT :
                                                                                depth == CV_32S ? NPY_INT : depth == CV_32F ? NPY_FLOAT :
                                                                                                            depth == CV_64F ? NPY_DOUBLE : f*NPY_ULONGLONG + (f^1)*NPY_UINT;
      int i;
      npy_intp _sizes[CV_MAX_DIM+1];
      for( i = 0; i < dims; i++ )
      {
        _sizes[i] = sizes[i];
      }

      if( cn > 1 )
      {
        _sizes[dims++] = cn;
      }
      PyObject* o = PyArray_SimpleNew(dims, _sizes, typenum);
      if(!o)
      {
        CV_Error_(CV_StsError, ("The numpy array of typenum=%d, ndims=%d can not be created", typenum, dims));
      }
      refcount = refcountFromPyObject(o);
      PyArrayObject* oCasted = reinterpret_cast<PyArrayObject*>(o);
      npy_intp* _strides = PyArray_STRIDES(oCasted); // added oCasted
      for( i = 0; i < dims - (cn > 1); i++ )
        step[i] = (size_t)_strides[i];
      datastart = data = (uchar*)PyArray_DATA(oCasted); // added oCasted
    }

    void deallocate(int* refcount, uchar*, uchar*)
    {
      //PyEnsureGIL gil;
      if( !refcount )
        return;

      PyObject* o = pyObjectFromRefcount(refcount);
      Py_INCREF(o);
      Py_DECREF(o);
    }

};
#else // CV_MAJOR_VERSION
#if CV_MAJOR_VERSION < 4
class cedar::proc::steps::PythonScriptScope::NumpyAllocator : public cv::MatAllocator
{
public:

    const MatAllocator* stdAllocator;
    NumpyAllocator() { stdAllocator = cv::Mat::getStdAllocator(); }
    ~NumpyAllocator() {}

    cv::UMatData* allocate(PyObject* o, int dims, const int* sizes, int type, size_t* step) const
    {
        cv::UMatData* u = new cv::UMatData(this);
        u->data = u->origdata = (uchar*)PyArray_DATA((PyArrayObject*)o);
        npy_intp* _strides = PyArray_STRIDES((PyArrayObject*)o);
        for (int i = 0; i < dims - 1; i++)
            step[i] = (size_t)_strides[i];
        step[dims - 1] = CV_ELEM_SIZE(type);
        u->size = sizes[0] * step[0];
        u->userdata = o;
        return u;
    }

    cv::UMatData* allocate(int dims0, const int* sizes, int type, void* data, size_t* step, int flags, cv::UMatUsageFlags usageFlags) const// CV_OVERRIDE
    {
        if (data != 0)
        {
            // issue #6969: CV_Error(Error::StsAssert, "The data should normally be NULL!");
            // probably this is safe to do in such extreme case
            return stdAllocator->allocate(dims0, sizes, type, data, step, flags, usageFlags);
        }
        //PyEnsureGIL gil;

        int depth = CV_MAT_DEPTH(type);
        int cn = CV_MAT_CN(type);
        const int f = (int)(sizeof(size_t) / 8);
        int typenum = depth == CV_8U ? NPY_UBYTE : depth == CV_8S ? NPY_BYTE :
            depth == CV_16U ? NPY_USHORT : depth == CV_16S ? NPY_SHORT :
            depth == CV_32S ? NPY_INT : depth == CV_32F ? NPY_FLOAT :
            depth == CV_64F ? NPY_DOUBLE : f * NPY_ULONGLONG + (f ^ 1) * NPY_UINT;
        int i, dims = dims0;
        cv::AutoBuffer<npy_intp> _sizes(dims + 1);
        for (i = 0; i < dims; i++)
            _sizes[i] = sizes[i];
        if (cn > 1)
            _sizes[dims++] = cn;
        PyObject* o = PyArray_SimpleNew(dims, _sizes, typenum);
        if (!o)
            CV_Error_(cv::Error::StsError, ("The numpy array of typenum=%d, ndims=%d can not be created", typenum, dims));
        return allocate(o, dims0, sizes, type, step);
    }

    bool allocate(cv::UMatData* u, int accessFlags, cv::UMatUsageFlags usageFlags) const //CV_OVERRIDE
    {
        return stdAllocator->allocate(u, accessFlags, usageFlags);
    }

    void deallocate(cv::UMatData* u) const //CV_OVERRIDE
    {
        if (!u)
            return;
        //PyEnsureGIL gil;
        CV_Assert(u->urefcount >= 0);
        CV_Assert(u->refcount >= 0);
        if (u->refcount == 0)
        {
            PyObject* o = (PyObject*)u->userdata;
            Py_XDECREF(o);
            delete u;
        }
    }

};
#else 
class cedar::proc::steps::PythonScriptScope::NumpyAllocator : public cv::MatAllocator
{
public:

    NumpyAllocator() { stdAllocator = cv::Mat::getStdAllocator(); }
    ~NumpyAllocator() {}

    cv::UMatData* allocate(PyObject* o, int dims, const int* sizes, int type, size_t* step) const
    {

        cv::UMatData* u = new cv::UMatData(this);
        u->data = u->origdata = (uchar*)PyArray_DATA((PyArrayObject*)o);
        npy_intp* _strides = PyArray_STRIDES((PyArrayObject*)o);
        for (int i = 0; i < dims - 1; i++)
            step[i] = (size_t)_strides[i];
        step[dims - 1] = CV_ELEM_SIZE(type);
        u->size = sizes[0] * step[0];
        u->userdata = o;
        return u;
    }

    cv::UMatData* allocate(int dims0, const int* sizes, int type, void* data, size_t* step, cv::AccessFlag flags, cv::UMatUsageFlags usageFlags) const// CV_OVERRIDE
    {
        if (data != 0)
        {
            // issue #6969: CV_Error(Error::StsAssert, "The data should normally be NULL!");
            // probably this is safe to do in such extreme case
            return stdAllocator->allocate(dims0, sizes, type, data, step, flags, usageFlags);
        }
        //PyEnsureGIL gil;

        int depth = CV_MAT_DEPTH(type);
        int cn = CV_MAT_CN(type);
        const int f = (int)(sizeof(size_t) / 8);
        int typenum = depth == CV_8U ? NPY_UBYTE : depth == CV_8S ? NPY_BYTE :
            depth == CV_16U ? NPY_USHORT : depth == CV_16S ? NPY_SHORT :
            depth == CV_32S ? NPY_INT : depth == CV_32F ? NPY_FLOAT :
            depth == CV_64F ? NPY_DOUBLE : f * NPY_ULONGLONG + (f ^ 1) * NPY_UINT;
        int i, dims = dims0;
        cv::AutoBuffer<npy_intp> _sizes(dims + 1);
        for (i = 0; i < dims; i++)
            _sizes[i] = sizes[i];
        if (cn > 1)
            _sizes[dims++] = cn;
        PyObject* o = PyArray_SimpleNew(dims, _sizes, typenum);
        if (!o)
            CV_Error_(cv::Error::StsError, ("The numpy array of typenum=%d, ndims=%d can not be created", typenum, dims));
        return allocate(o, dims0, sizes, type, step);
    }

    bool allocate(cv::UMatData* u, cv::AccessFlag accessFlags, cv::UMatUsageFlags usageFlags) const //CV_OVERRIDE
    {
        return stdAllocator->allocate(u, accessFlags, usageFlags);
    }

    void deallocate(cv::UMatData* u) const //CV_OVERRIDE
    {
        if (!u)
            return;
        //PyEnsureGIL gil;
        CV_Assert(u->urefcount >= 0);
        CV_Assert(u->refcount >= 0);
        if (u->refcount == 0)
        {
            PyObject* o = (PyObject*)u->userdata;
            Py_XDECREF(o);
            delete u;
        }
    }

    const MatAllocator* stdAllocator;

};
#endif
#endif // CV_MAJOR_VERSION

cedar::proc::steps::PythonScriptScope::NumpyAllocator g_numpyAllocator;


// static member decleration
int cedar::proc::steps::PythonScriptScope::NDArrayConverter::isInitialized = 0;


cedar::proc::steps::PythonScriptScope::NDArrayConverter::NDArrayConverter()
:
pythonScript(nullptr)
{
  init();
}

cedar::proc::steps::PythonScriptScope::NDArrayConverter::NDArrayConverter(cedar::proc::steps::PythonScript* pScript)
:
pythonScript(pScript)
{
  init();
}

cedar::proc::steps::PythonScriptScope::ValidationMaskInputDialog::~ValidationMaskInputDialog()
{

}

void cedar::proc::steps::PythonScriptScope::NDArrayConverter::init()
{
  if(!NDArrayConverter::isInitialized)
  {
    NDArrayConverter::isInitialized = 1;
    _import_array();
  }
}

const char * cedar::proc::steps::PythonScriptScope::NDArrayConverter::typenumToString(int typenum) {
  switch(typenum){
    case NPY_BOOL:
      return "boolean (NPY_BOOL)";
    case NPY_UBYTE:
      return "unsigned byte (NPY_UBYTE)";
    case NPY_BYTE:
      return "byte (NPY_BYTE)";
    case NPY_USHORT:
      return "unsigned short (NPY_USHORT)";
    case NPY_SHORT:
      return "short (NPY_SHORT)";
    case NPY_UINT:
      return "unsigned int (NPY_UINT)";
    case NPY_INT:
      return "int (NPY_INT)";
    case NPY_ULONGLONG:
      return "unsigned long long (NPY_ULONGLONG)";
    case NPY_LONGLONG:
      return "long long (NPY_LONGLONG)";
    case NPY_HALF:
      return "float16 (NPY_HALF)";
    case NPY_FLOAT:
      return "float32 (NPY_FLOAT)";
    case NPY_DOUBLE:
      return "double (NPY_DOUBLE)";
    case NPY_LONGDOUBLE:
      return "long double (NPY_LONGDOUBLE)";
    case NPY_CFLOAT:
      return "complex float32 (NPY_CFLOAT)";
    case NPY_CDOUBLE:
      return "complex double (NPY_CDOUBLE)";
    case NPY_CLONGDOUBLE:
      return "complex long double (NPY_CLONGDOUBLE)";
    case NPY_DATETIME:
      return "datetime (NPY_DATETIME)";
    case NPY_TIMEDELTA:
      return "timedelta (NPY_TIMEDELTA)";
    case NPY_STRING:
      return "string (NPY_STRING)";
    case NPY_UNICODE:
      return "unicode (NPY_UNICODE)";
    case NPY_OBJECT:
      return "object (NPY_OBJECT)";
    case NPY_VOID:
      return "void (NPY_VOID)";
    case NPY_NOTYPE:
      return "notype (NPY_NOTYPE)";
    case NPY_USERDEF:
      return "user defined (NPY_USERDEF)";
    default:
      return "unknown";
  }

}

cv::Mat cedar::proc::steps::PythonScriptScope::NDArrayConverter::toMat(PyObject* o, int index)
{
  cv::Mat m;
  bool allowND = true;
  if(!o || o == Py_None)
  {
    return cv::Mat::zeros(1,1,CV_32F);
  }

#if PY_MAJOR_VERSION >= 3
  if( PyLong_Check(o) )
  {
    double v[] = {static_cast<double>(PyLong_AsLong((PyObject*)o))};
#else // PY_MAJOR_VERSION >= 3
  if( PyInt_Check(o) )
  {
  double v[] = {static_cast<double>(PyInt_AsLong((PyObject*)o))};
#endif // PY_MAJOR_VERSION >= 3

    m = cv::Mat(1, 1, CV_64F, v).clone();
    return m;
  }
  if( PyFloat_Check(o) )
  {
    double v[] = {PyFloat_AsDouble((PyObject*)o)};
    m = cv::Mat(1, 1, CV_64F, v).clone();
    return m;
  }
  if( PyTuple_Check(o) )
  {
    int i, sz = (int)PyTuple_Size((PyObject*)o);
    m = cv::Mat(sz, 1, CV_64F);
    for( i = 0; i < sz; i++ )
    {
      PyObject* oi = PyTuple_GetItem(o, i);


#if PY_MAJOR_VERSION >= 3
      if( PyLong_Check(oi) )
      {
        m.at<double>(i) = (double) PyLong_AsLong(oi);
#else // PY_MAJOR_VERSION >= 3
      if( PyInt_Check(oi) )
      {
        m.at<double>(i) = (double) PyInt_AsLong(oi);
#endif // PY_MAJOR_VERSION >= 3
      }
      else if( PyFloat_Check(oi) )
      {
        m.at<double>(i) = (double) PyFloat_AsDouble(oi);
      }
      else
      {
        failmsg("Object in pc.outputs[%d] is not a numerical tuple.", index);
        return cv::Mat::zeros(1,1,CV_32F);
      }
    }
    return m;
  }

  if( !PyArray_Check(o) )
  {
    failmsg("Object in pc.outputs[%d] is not a numpy array, neither a scalar.", index);
    return cv::Mat::zeros(1,1,CV_32F);
  }

  PyArrayObject* oarr = (PyArrayObject*) o;

  bool needcopy = false, needcast = false;
  int typenum = PyArray_TYPE(oarr), new_typenum = typenum;
  int type = typenum == NPY_UBYTE ? CV_8U :
             typenum == NPY_BYTE ? CV_8S :
             typenum == NPY_USHORT ? CV_16U :
             typenum == NPY_SHORT ? CV_16S :
             typenum == NPY_INT ? CV_32S :
             typenum == NPY_INT32 ? CV_32S :
             typenum == NPY_FLOAT ? CV_32F :
             typenum == NPY_DOUBLE ? CV_64F : -1;

  if( type < 0 )
  {
    if( typenum == NPY_INT64 || typenum == NPY_UINT64 || typenum == NPY_LONG )
    {
      needcopy = needcast = true;
      new_typenum = NPY_INT;
      type = CV_32S;
    }
    else
    {
      failmsg("Data type (%s) of matrix in pc.outputs[%d] is not supported.", typenumToString(typenum), index);
      return cv::Mat::zeros(1,1,CV_32F);
    }
  }

#ifndef CV_MAX_DIM
  const int CV_MAX_DIM = 32;
#endif

  int ndims = PyArray_NDIM(oarr);
  if(ndims >= CV_MAX_DIM)
  {
    failmsg("Dimension of matrix in pc.outputs[%d] is too high. (%d)", index, ndims);
    return cv::Mat::zeros(1,1,CV_32F);
  }

  int size[CV_MAX_DIM+1];
  size_t step[CV_MAX_DIM+1];
  size_t elemsize = CV_ELEM_SIZE1(type);
  const npy_intp* _sizes = PyArray_DIMS(oarr);
  const npy_intp* _strides = PyArray_STRIDES(oarr);
  bool ismultichannel = ndims == 3 && _sizes[2] <= CV_CN_MAX;

  for( int i = ndims-1; i >= 0 && !needcopy; i-- )
  {
    // these checks handle cases of
    //  a) multi-dimensional (ndims > 2) arrays, as well as simpler 1- and 2-dimensional cases
    //  b) transposed arrays, where _strides[] elements go in non-descending order
    //  c) flipped arrays, where some of _strides[] elements are negative
    // the _sizes[i] > 1 is needed to avoid spurious copies when NPY_RELAXED_STRIDES is set
    if( (i == ndims-1 && _sizes[i] > 1 && (size_t)_strides[i] != elemsize) ||
        (i < ndims-1 && _sizes[i] > 1 && _strides[i] < _strides[i+1]) )
    {
      needcopy = true;
    }
  }

  if( ismultichannel && _strides[1] != (npy_intp)elemsize*_sizes[2] )
  {
    needcopy = true;
  }

  if (needcopy)
  {
    if( needcast )
    {
      o = PyArray_Cast(oarr, new_typenum);
      oarr = (PyArrayObject*) o;
    }
    else
    {
      oarr = PyArray_GETCONTIGUOUS(oarr);
      o = (PyObject*) oarr;
    }

    _strides = PyArray_STRIDES(oarr);
  }

  // Normalize strides in case NPY_RELAXED_STRIDES is set
  size_t default_step = elemsize;
  for ( int i = ndims - 1; i >= 0; --i )
  {
    size[i] = (int)_sizes[i];
    if ( size[i] > 1 )
    {
      step[i] = (size_t)_strides[i];
      default_step = step[i] * size[i];
    }
    else
    {
      step[i] = default_step;
      default_step *= size[i];
    }
  }

  // handle degenerate case
  if(ndims == 0)
  {
    failmsg("Matrix in pc.outputs[%d] has 0 dimensions.", index);
    return cv::Mat::zeros(1,1,CV_32F);

    // Could check the following:
    //size[ndims] = 1;
    //step[ndims] = elemsize;
    //ndims++;
  }
  //if output is an rgb image, do multichannel conversion
  if(ismultichannel && (type == CV_8U || type == CV_8UC3))
  {
    int newType = type | CV_MAKETYPE(0, size[2]);
    if(newType == CV_8UC3)
    {
      ndims--;
      type = newType;
    }
    else
    {
      std::cout << "PythonScript: Multichannel conversion failed. Trying to convert from type " << type << " to " << newType << ". Add support for this type if multichannel conversion is intended." << std::endl;
    }
  }

  if( ndims > 2 && !allowND )
  {
    failmsg("Matrix in pc.outputs[%d] has more than 2 dimensions. (%d) (This is currently not supported)", index, ndims);
    return cv::Mat::zeros(1,1,CV_32F);
  }

  m = cv::Mat(ndims, size, type, PyArray_DATA(oarr), step);

#if CV_MAJOR_VERSION < 3
  if( m.data )
  {
    m.refcount = refcountFromPyObject(o);
    m.addref(); // protect the original numpy array from deallocation
    // (since Mat destructor will decrement the reference counter)
  }
#else // CV_MAJOR_VERSION
  m.u = g_numpyAllocator.allocate(o, ndims, size, type, step);
  m.addref();
  if( !needcopy )
  {
    Py_INCREF(o);
  }
#endif // CV_MAJOR_VERSION

  m.allocator = &g_numpyAllocator;
  return m;
}

PyObject* cedar::proc::steps::PythonScriptScope::NDArrayConverter::toNDArray(const cv::Mat& m)
{
  if( !m.data )
  {
    Py_RETURN_NONE;
  }
  cv::Mat temp, *p = (cv::Mat*)&m;

#if CV_MAJOR_VERSION < 3
  if(!p->refcount || p->allocator != &g_numpyAllocator)
#else
  if(!p->u || p->allocator != &g_numpyAllocator)
#endif
  {
    temp.allocator = &g_numpyAllocator;
    try
    {
      //PyAllowThreads allowThreads;
      m.copyTo(temp);
    }
    catch (const cv::Exception &e)
    {
      PyErr_SetString(0, e.what());
      return 0;
    }
    //m.copyTo(temp);
    p = &temp;
  }
#if CV_MAJOR_VERSION < 3
  p->addref();
  return pyObjectFromRefcount(p->refcount);
#else
  PyObject* o = (PyObject*)p->u->userdata;
  Py_INCREF(o);
  return o;
#endif
}



void printFromPython(PyObject* obj)
{
  char const* text = boost::python::extract<const char*>(obj);

  std::string name = cedar::proc::steps::PythonScript::nameOfExecutingStep;
  boost::trim(name);
  if(name == "") name = "Python Script";

  cedar::aux::LogSingleton::getInstance()->message
        (
          std::string("[PythonScript] ").append(text),
          "void cedar::proc::steps::PythonScript::printFromPython(char const* text)",
          name
        );
   
}

struct pylist_converter
{
    static void* convertible(PyObject* object)
    {
      if (!PyList_Check(object)) {
        return nullptr;
      }
      return object;
    }

    static void construct(PyObject* object, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      typedef boost::python::converter::rvalue_from_python_storage<std::list<PyObject*>> storage_type;
      void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

      data->convertible = new (storage) std::list<PyObject*>();

      std::list<PyObject*>* l = (std::list<PyObject*>*)(storage);

      int sz = PySequence_Size(object);
      for (int i = 0; i < sz; ++i) {
        l->push_back(PyList_GetItem(object, i));
      }
    }
};

struct std_list_to_python
{
    static PyObject* convert(std::list<boost::python::handle<>> const& l)
    {
      boost::python::list result;
      for (auto const& value : l) {
        result.append(value);
      }
      return boost::python::incref(result.ptr());
    }
};

BOOST_PYTHON_MODULE(pycedar)
{
  // Tried to expose vectors of cv::Mat
  // boost::python::class_<std::vector<boost::python::handle<> > >("MatVec")
  //       .def(boost::python::vector_indexing_suite<std::vector<boost::python::handle<> > >());

  boost::python::to_python_converter<std::list<boost::python::handle<>>, std_list_to_python, false>();

  boost::python::converter::registry::push_back(&pylist_converter::convertible
          ,&pylist_converter::construct
          ,boost::python::type_id<std::list<PyObject*>>());
        
  boost::python::def("messagePrint", &printFromPython);
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::PythonScript::PythonScript()
:
PythonScript(false)
{

}


cedar::proc::steps::PythonScript::PythonScript(bool isLooped)
:
cedar::proc::Step(isLooped)
,
mInputs(1, cedar::aux::MatDataPtr()),
mOutputs(1, cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))),
mStates(0, cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))),
mWasResetted(true),

// Declare Properties
_mCodeStringForSavingArchitecture (new cedar::aux::StringParameter(this, "code", "import numpy as np\nimport pycedar as pc\n\n#Print to messages tab:\n# pc.messagePrint('text')\n# pc.messagePrint(str(...))\n\n#Inputs: (List of NumPy Arrays)\n# pc.inputs[0]\n# pc.inputs[1]\n# ...\n\n#Outputs: (List of NumPy Arrays)\n# pc.outputs[0]\n# pc.outputs[1]\n# ...\n\n\ninput1 = pc.inputs[0]\n\npc.outputs[0] = input1 * 2\n\n\n# Experimental:\n# A state that is kept between executions:\n# if len(pc.states) == 0:\n#   pc.states.append( 41 )\n# pc.states[0]= pc.states[0] + 1\n\n# Experimental:\n# Did a reset just occur?\n# if pc.reset:\n#   pc.messagePrint('reset occured')\n#   pc.states[0]= 41\n")),
_mNumberOfInputs (new cedar::aux::UIntParameter(this, "number of inputs", 1,0,255)),
_mNumberOfOutputs (new cedar::aux::UIntParameter(this, "number of outputs", 1,0,255)),
_mHasScriptFile (new cedar::aux::BoolParameter(this, "use script file", false)),
_mScriptFile (new cedar::aux::FileParameter(this, "script file path", cedar::aux::FileParameter::READ)),
_mAutoConvertDoubleToFloat (new cedar::aux::BoolParameter(this, "auto-convert double output matrices to float", true))
{
  this->declareInput(makeInputSlotName(0), false);
  
  this->declareOutput(makeOutputSlotName(0), mOutputs[0]);
  
  QObject::connect(this->_mHasScriptFile.get(), SIGNAL(valueChanged()), this, SLOT(hasScriptFileChanged()));
  QObject::connect(_mNumberOfInputs.get(), SIGNAL(valueChanged()), this, SLOT(numberOfInputsChanged()));
  QObject::connect(_mNumberOfOutputs.get(), SIGNAL(valueChanged()), this, SLOT(numberOfOutputsChanged()));

  this->_mScriptFile->setConstant(true);
  this->_mCodeStringForSavingArchitecture->setHidden(true);
  this->_mAutoConvertDoubleToFloat->markAdvanced(true);

  this->registerFunction("export step as template", boost::bind(&cedar::proc::steps::PythonScript::exportStepAsTemplate   , this), false);
  //this->registerFunction("import step from template", boost::bind(&cedar::proc::steps::PythonScript::importStepsFromTemplate, this), false);
  
  cedar::proc::steps::PythonScript::executionFailed = 0;

//#if PY_MAJOR_VERSION >= 3
//  PyImport_AppendInittab("pycedar", &PyInit_pycedar);
//#else // PY_MAJOR_VERSION >= 3
//  PyImport_AppendInittab("pycedar", &initpycedar);
//#endif // PY_MAJOR_VERSION >= 3
//  PyEval_InitThreads();
//  Py_Initialize();
  //_nstate = Py_NewInterpreter();
}

void cedar::proc::steps::PythonScript::initPython()
{
#if PY_MAJOR_VERSION >= 3
  PyImport_AppendInittab("pycedar", &PyInit_pycedar);
#else // PY_MAJOR_VERSION >= 3
  PyImport_AppendInittab("pycedar", &initpycedar);
#endif // PY_MAJOR_VERSION >= 3
  PyEval_InitThreads();
  Py_Initialize();
}


cedar::proc::steps::PythonScript::~PythonScript()
{

}

template<typename T>
cv::Mat cedar::proc::steps::PythonScript::convert3DMatToFloat(cv::Mat &mat)
{
  if(mat.dims != 3) return cv::Mat::zeros(1,1,CV_32F);
  int size[] = {mat.size[0], mat.size[1], mat.size[2]};
  cv::Mat output(3, size, CV_32F);
  for (int i = 0; i < size[0]; ++i)
  {
    for (int j = 0; j < size[1]; ++j)
    {
      for (int k = 0; k < size[2]; ++k)
      {
        output.at<float>(i,j,k) = static_cast<float>(mat.at<T>(i,j,k));
      }
    }
  }
  return output;
}

void cedar::proc::steps::PythonScript::hasScriptFileChanged()
{
  if(this->_mHasScriptFile->getValue())
  {
    this->_mScriptFile->setConstant(false);
  }
  else
  {
    this->_mScriptFile->setConstant(true);
  }
}

void cedar::proc::steps::PythonScript::numberOfInputsChanged()
{
  unsigned newsize = _mNumberOfInputs->getValue();
  if (newsize == mInputs.size())
  {
    return;
  }
  else if (newsize < mInputs.size())
  {
    // delete unused slots
    for (unsigned i=newsize; i<mInputs.size(); i++)
    {
      removeInputSlot(makeInputSlotName(i));
    }
  }
  else if (newsize > mInputs.size())
  {
    // declare new input slots
    for (unsigned i=mInputs.size(); i<newsize; i++)
    {
      this->declareInput(makeInputSlotName(i), false);
    }
  }
  // resize inputs vector
  mInputs.resize(newsize);
}

std::vector<cedar::proc::steps::PythonScript::TemplateName> cedar::proc::steps::PythonScript::getTemplateNames()
{
  std::vector<TemplateName> list;
  try
  {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(cedar::aux::Path("resource://pythonTemplates/python.json").absolute(), root);
    if(root.find("steps") != root.not_found())
    {
      for (boost::property_tree::ptree::value_type &nodeOuter : root.get_child("steps"))
      {
        std::string left = nodeOuter.first;
        boost::trim(left);
        bool isLooped = false, isNonLooped = false;
        if (!left.compare("cedar.processing.steps.PythonScriptLooped")) isLooped = true;
        if (!left.compare("cedar.processing.steps.PythonScript")) isNonLooped = true;
        if (isLooped || isNonLooped)
        {
          for (boost::property_tree::ptree::value_type &node : nodeOuter.second)
          {
            std::string left = node.first;
            boost::trim(left);
            if (!left.compare("name"))
            {
              list.push_back(TemplateName());
              list[list.size() - 1].name = node.second.data();
              list[list.size() - 1].isLooped = isLooped;
            }
          }
        }
      }
    }
  }
  catch(boost::property_tree::ptree_bad_path e)
  {
    //std::cout << "Error[1]: " << e.what() << std::endl;
  }
  catch(boost::property_tree::json_parser::json_parser_error e)
  {
    //std::cout << "Error[2]: " << e.what() << std::endl;
  }
  catch(std::exception e)
  {
    //std::cout << "Error[3]: " << e.what() << std::endl;
  }

  return list;
}

void cedar::proc::steps::PythonScript::declareTemplate(const char* name, bool isLooped){
  std::string category = "Python Templates";

  cedar::proc::ElementDeclarationManagerSingleton::getInstance()->addCategory(category);

  cedar::proc::GroupDeclarationPtr python_declaration
          (
                  new cedar::proc::GroupDeclaration
                          (
                                  name,
                                  "resource://pythonTemplates/python.json",
                                  name,
                                  category
                          )
          );
  std::string icon_path;
  if(isLooped)
  {
    icon_path = ":/steps/python_script.svg";
  }
  else
  {
    icon_path = ":/steps/python_script.svg";
  }
  python_declaration->setIconPath(icon_path);
  python_declaration->setDescription
          (
                  "A template of the PythonScript Step."
          );
  python_declaration->declare();
}

void cedar::proc::steps::PythonScript::importStepsFromTemplate()
{
  std::vector<TemplateName> list = getTemplateNames();
  if(list.size() > 0)
  {
    for (unsigned int i = 0; i < list.size(); ++i)
    {
      std::string name = list.at(i).name;

      // Declare template if it is not already declared
      if(!cedar::proc::GroupDeclarationManagerSingleton::getInstance()->getDeclarationNoThrow(name)){
        declareTemplate(name.c_str(), list.at(i).isLooped);
      }
    }

    // calls the reset() function of ElementList, so the template is visible
    cedar::proc::gui::SettingsSingleton::getInstance()->emitElementListViewResetSignal();
  }
}

// Abstraction of writeJson()
void cedar::proc::steps::PythonScript::appendJson(cedar::aux::Path filename, bool isLooped, std::string templateName){
  // make sure the directory to which the file is supposed to be written exists
  filename.absolute().createDirectories();

  // generate the configuration tree
  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);
  boost::property_tree::ptree config = configuration;
  config.put("name", templateName);

  boost::property_tree::ptree root;

  // Read existing json
  try
  {
    boost::property_tree::read_json(filename.absolute(), root);

    std::string name;
    if(isLooped)
    {
      name = "cedar.processing.steps.PythonScriptLooped";
    }
    else
    {
      name = "cedar.processing.steps.PythonScript";
    }

    boost::optional<boost::property_tree::ptree&> steps = root.get_child_optional("steps");
    // If the "steps" tree is already there, append our configuration
    if(steps)
    {
      root.get_child("steps").add_child(boost::property_tree::ptree::path_type(name, '&'),config);
    }
    else
    {
      boost::property_tree::ptree stepsTree;
      stepsTree.add_child(boost::property_tree::ptree::path_type(name, '&'),config);
      root.add_child("steps",stepsTree);
    }


    // write the tree to a file
    boost::property_tree::write_json(filename.absolute(), root);
  }
  catch(boost::property_tree::ptree_bad_path e)
  {
    //std::cout << "Error[1]: " << e.what() << std::endl;
  }
  catch(boost::property_tree::json_parser::json_parser_error e)
  {
    //std::cout << "Error[2]: " << e.what() << std::endl;
  }
  catch(std::exception e)
  {
    //std::cout << "Error[3]: " << e.what() << std::endl;
  }
}

void cedar::proc::steps::PythonScript::exportStepAsTemplate()
{
  std::vector<TemplateName> templateNames = getTemplateNames();

  bool ok;
  // create the list of not accepted strings
  std::vector<std::string> unaccepted;
  unaccepted.push_back("");
  for(unsigned int i = 0; i < templateNames.size(); i++)
  {
    unaccepted.push_back(templateNames.at(i).name);
  }

  // execute the dialog
  cedar::proc::steps::PythonScriptScope::ValidationMaskInputDialog dialog(unaccepted);
  QString text = dialog.getText(0, "Export step as Template", "Name for the template:", QLineEdit::Normal, QString::fromStdString(this->getName()), &ok);

  if(!ok)
  {
    return;
  }

  // Check again if no name breaks the name rules
  std::string exportTemplateName = text.toStdString();
  boost::trim(exportTemplateName);
  if(!exportTemplateName.compare(""))
  {
    CEDAR_THROW(cedar::aux::ParseException, "The template name cannot be null");
    return;
  }
  for (unsigned int i = 0; i < templateNames.size(); ++i)
  {
    if(!templateNames.at(i).name.compare(exportTemplateName))
    {
      CEDAR_THROW(cedar::aux::DuplicateNameException, "A template with name \"" + exportTemplateName + "\" already exists.");
      return;
    }
  }

  // export the template
  appendJson(cedar::aux::Path("resource://pythonTemplates/python.json"), isLooped(), exportTemplateName);

  // Load templates, to make the exported step visible
  importStepsFromTemplate();
}

void cedar::proc::steps::PythonScript::numberOfOutputsChanged()
{
  unsigned newsize = _mNumberOfOutputs->getValue();
  unsigned oldsize = mOutputs.size();
  if (newsize == oldsize)
  {
    return;
  }
  
  // resize outputs vector
  mOutputs.resize(newsize);
  
  if (newsize < oldsize)
  {
    // delete unused slots
    for (unsigned i=newsize; i<oldsize; i++)
    {
      removeOutputSlot(makeOutputSlotName(i));
    }
  }
  else if (newsize > oldsize)
  {
    // declare new input slots
    for (unsigned i=oldsize; i<newsize; i++)
    {
      mOutputs[i] = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)));
      this->declareOutput(makeOutputSlotName(i), mOutputs[i]);
    }
  }
  
}

std::string cedar::proc::steps::PythonScript::makeInputSlotName(const int i)
{
    std::stringstream s;
    s << "input " << i;
    return s.str();
}

std::string cedar::proc::steps::PythonScript::makeOutputSlotName(const int i)
{
    std::stringstream s;
    s << "output " << i;
    return s.str();
}

void cedar::proc::steps::PythonScript::freePythonVariables() {
  PyObject * poMainModule = PyImport_AddModule("__main__");

  PyObject * poAttrList = PyObject_Dir(poMainModule);

  PyObject * poAttrIter = PyObject_GetIter(poAttrList);

  PyObject * poAttrName;

  while ((poAttrName = PyIter_Next(poAttrIter)) != NULL)
  {
    std::string oAttrName;
    if (PyUnicode_Check(poAttrName))
    {
      PyObject* temp = PyUnicode_AsASCIIString(poAttrName);
      if (NULL == temp) {
        std::cout << "unicode to ASCII conversion failed" << std::endl;
        oAttrName = "__asd__";
      }
      else
      {
#if PY_MAJOR_VERSION >= 3
        oAttrName = PyBytes_AsString(temp);
#else // PY_MAJOR_VERSION >= 3
        oAttrName = PyByteArray_AsString(temp);
#endif // PY_MAJOR_VERSION >= 3
      }
      Py_DecRef(temp);
    }
    else
    {
#if PY_MAJOR_VERSION >= 3
      oAttrName = PyBytes_AsString(poAttrName);
#else // PY_MAJOR_VERSION >= 3
      oAttrName = PyString_AsString(poAttrName);
#endif // PY_MAJOR_VERSION >= 3
    }
    // Make sure we don't delete any private objects.
    if (!boost::starts_with(oAttrName, "__") || !boost::ends_with(oAttrName, "__"))
    {
      PyObject * poAttr = PyObject_GetAttr(poMainModule, poAttrName);

      // Make sure we don't delete any module objects.
      if (poAttr && poAttr->ob_type != poMainModule->ob_type)
        PyObject_SetAttr(poMainModule, poAttrName, NULL);

      Py_DecRef(poAttr);
    }

    Py_DecRef(poAttrName);
  }

  Py_DecRef(poAttrIter);

  Py_DecRef(poAttrList);
}

void cedar::proc::steps::PythonScript::executePythonScript(bool use_data_lock)
{

  mutex.lock();
  //PyThreadState_Swap(_nstate);

  //Interpreter
  //cedar::proc::steps::PythonScriptScope::PyEnsureGIL gil;
  this->mIsExecuting = 1;
  cedar::proc::steps::PythonScript::executionFailed = 0;

  nameOfExecutingStep = this->getName();

  // Swap to the interpreter of this specific PythonScript step
  // Python...
  try
  {
    // Loading main module
    //boost::python::object main_module = boost::python::import("__main__");
    boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__")))));
    boost::python::object main_namespace = main_module.attr("__dict__");

    // Loading pycedar module
    boost::python::object pycedar_module( (boost::python::handle<>(PyImport_ImportModule("pycedar"))) );

    std::list<boost::python::handle<>> inputsList;
    cedar::proc::steps::PythonScriptScope::NDArrayConverter cvt(this);
    cedar::aux::annotation::ConstColorSpacePtr pColorSpaceAnnotation = nullptr;
    for(unsigned int i = 0; i < mInputs.size(); i++)
    {
      cedar::aux::ConstDataPtr inputMatrixPointer = this->getInputSlot(makeInputSlotName(i))->getData();
      if(inputMatrixPointer != 0)
      {

        try
        {
          if(pColorSpaceAnnotation == nullptr)
          {
            pColorSpaceAnnotation = inputMatrixPointer->getAnnotation<cedar::aux::annotation::ColorSpace>();
          }
        }
        catch(std::exception)
        {
          pColorSpaceAnnotation = nullptr;
        }

        auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(inputMatrixPointer);

        cv::Mat inputMatrix;

        if (use_data_lock)
        {
          // Lock input matrix
          // (do not lock when calling from compute())
          QReadLocker input_l(&mat_data->getLock());
          inputMatrix = mat_data->getData().clone();
        }
        else
        {
          inputMatrix = mat_data->getData().clone();
        }

        // Convert input matrix to numpy array
        PyObject* inputMatrix_np = cvt.toNDArray(inputMatrix);
        // Include it to Python
        boost::python::handle<> inputMatrix_np_handle(inputMatrix_np);
        inputsList.push_back(inputMatrix_np_handle);

        // Old way: Setting the Input to pc.input1, pc.input2 etc.
        /*

        boost::python::object inputMatrix_object(inputMatrix_np_handle);
        // Setting the input directly in the global pycedar scope
        const char* attrName = std::string("input").append(std::to_string(i + 1)).c_str();
        boost::python::scope(pycedar_module).attr(attrName) = inputMatrix_object;

        */
      }
      else
      {
        // Old way: Setting the Input to pc.input1, pc.input2 etc.
        /*

        const char* attrName = std::string("input").append(std::to_string(i + 1)).c_str();
        boost::python::scope(pycedar_module).attr(attrName) = -1;

        */

        PyObject* matrix_np = cvt.toNDArray(cv::Mat::zeros(1, 1, CV_32F));
        boost::python::handle<> matrix_np_handle(matrix_np);
        inputsList.push_back(matrix_np_handle);

      }
    }
    boost::python::scope(pycedar_module).attr("inputs") = inputsList;

    // fill the "states" matrix:
    std::list<boost::python::handle<>>  statesListBefore;
    for(unsigned int i = 0; i < mStates.size(); i++)
    {
      cv::Mat stateMatrix= mStates[i]->getData(); //.clone(); // do we need clone?

      PyObject* stateMatrix_np = cvt.toNDArray(stateMatrix);
      boost::python::handle<> stateMatrix_np_handle(stateMatrix_np);
      statesListBefore.push_back(stateMatrix_np_handle);
    }
    boost::python::scope(pycedar_module).attr("states") = statesListBefore;

    // add the 'just resetted' information
    boost::python::scope(pycedar_module).attr("reset") = mWasResetted;


    // Adding empty matrices to the output list so that the length of the list in python is correctly set
    std::list<boost::python::handle<>> outputsList;
    boost::python::handle<> hand(cvt.toNDArray( cv::Mat::zeros(1, 1, CV_32F) ));

    for(unsigned int i = 0; i < mOutputs.size(); i++)
      outputsList.push_back(hand);

    boost::python::scope(pycedar_module).attr("outputs") = outputsList;
    // Old way: Setting the Output to pc.output1, pc.output2 etc.
    /*

    // Remove output data of old calls
    for(unsigned int i = 0; i < mOutputs.size(); i++){
      const char* attrName = std::string("output").append(std::to_string(i + 1)).c_str();

      // Test if the output object exists in the pycedar scope
      try
      {
        boost::python::scope(pycedar_module).attr(attrName);
      }
      catch(const boost::python::error_already_set&)
      {
        continue;
      }

      // Create empty matrix
      PyObject* emptyMatrix_np = cvt.toNDArray(cv::Mat::zeros(1, 1, CV_32F));

      // Include it to Python
      boost::python::handle<> emptyMatrix_np_handle(emptyMatrix_np);
      boost::python::object emptyMatrix_object(emptyMatrix_np_handle);

      boost::python::scope(pycedar_module).attr(attrName) = emptyMatrix_object;
    }

    */

    bool executedFromFile = false;
    // Execute script from the specified file, if the hasScriptFile Parameter is set and the file exists
    if(this->_mHasScriptFile->getValue())
    {
      cedar::aux::Path path = this->_mScriptFile->getPath();
      if(path.exists()){
        boost::python::exec_file(path.toString().c_str(), main_namespace);
        executedFromFile = true;
      }
    }
    
    if(!executedFromFile)
    {
      std::string inputString = this->_mCodeStringForSavingArchitecture->getValue();
      boost::python::str inputString_py(inputString);
#if CV_MAJOR_VERSION < 3
      boost::python::exec(inputString_py, main_namespace);
#else
      boost::python::exec(boost::python::extract<char const*>(inputString_py), main_namespace);
#endif
    }

    // Old way: Setting the Output to pc.output1, pc.output2 etc.
    /*

    // relegate output from python to the step output
    for(unsigned int i = 0; i < mOutputs.size(); i++){
      const char* attrName = std::string("output").append(std::to_string(i + 1)).c_str();
      boost::python::object output_object;

      // Test if the output object exists in the pycedar scope
      try
      {
        output_object = boost::python::scope(pycedar_module).attr(attrName);
      }
      catch(const boost::python::error_already_set&)
      {
        continue;
      }

      PyObject * outputPointer = output_object.ptr();
      cv::Mat &outputNodeMatrix = mOutputs[i]->getData();
      outputNodeMatrix = cvt.toMat(outputPointer);
      // Convert 1D and 2D matrices of type CV_64F (double) to CV_32F (float)
      if (this->_autoConvertDoubleToFloat->getValue())
      {
        if (outputNodeMatrix.dims <= 2 &&
            outputNodeMatrix.type() == CV_64F)
        {
          outputNodeMatrix.convertTo(outputNodeMatrix, CV_32F); // added
        }
      }
    }

    */

    // Get the outputs from python
    std::list<PyObject*> list = boost::python::extract<std::list<PyObject*>>(boost::python::scope(pycedar_module).attr("outputs"));

    unsigned int i = 0;
    for (auto const& outputPointer : list) {
      if(i >= mOutputs.size()) break;

      cv::Mat &outputNodeMatrix = mOutputs[i]->getData();
      outputNodeMatrix = cvt.toMat(outputPointer, i);

      // Convert 1D and 2D matrices of type CV_64F (double) to CV_32F (float)
      if (this->_mAutoConvertDoubleToFloat->getValue())
      {
        if (outputNodeMatrix.dims <= 2 &&
            outputNodeMatrix.type() == CV_64F)
        {
          outputNodeMatrix.convertTo(outputNodeMatrix, CV_32F); // added
        }
        else if (outputNodeMatrix.dims == 3 &&
             outputNodeMatrix.type() == CV_64F)
        {
          outputNodeMatrix = convert3DMatToFloat<double>(outputNodeMatrix);
        }
      }
      if(outputNodeMatrix.type() == CV_8UC3 && pColorSpaceAnnotation != nullptr)
      {
        mOutputs[i]->setAnnotation(cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(*pColorSpaceAnnotation)));
      }
      else
      {
        mOutputs[i]->removeAnnotations<cedar::aux::annotation::ColorSpace>();
      }
      i++;
    }

    // update the "state" variable. It will be held until the next iteration
    std::list<PyObject*> pythonStatesList = boost::python::extract<std::list<PyObject*>>(boost::python::scope(pycedar_module).attr("states"));

    auto pythonStateSize= pythonStatesList.size();
    auto oldStateSize= mStates.size();

    if (pythonStateSize != oldStateSize)
    {
      mStates.clear();
      for(unsigned int j=0; j < pythonStateSize; j++)
      {
        mStates.push_back( cedar::aux::MatDataPtr(new cedar::aux::MatData() ) );
        //cv::Mat::zeros(1, 1, CV_32F)))
      }
    }

    i = 0;
    for (auto const& pythonStatesPointer : pythonStatesList)
    {
      cv::Mat &stateMatrix = mStates[i]->getData();
      stateMatrix = cvt.toMat(pythonStatesPointer, i); // overwriting

      // Convert 1D and 2D matrices of type CV_64F (double) to CV_32F (float)
      if (this->_mAutoConvertDoubleToFloat->getValue())
      {
        if (stateMatrix.dims <= 2
            && stateMatrix.type() == CV_64F)
        {
          stateMatrix.convertTo(stateMatrix, CV_32F);
        }
        else if (stateMatrix.dims == 3
                 && stateMatrix.type() == CV_64F)
        {
          stateMatrix = convert3DMatToFloat<double>(stateMatrix);
        }
      }
      i++;
    }
  }
  catch(const boost::python::error_already_set&){

    cedar::proc::steps::PythonScript::executionFailed = 1;

    if(PyErr_Occurred() == 0) std::cout << "No PyErr occured!" << std::endl;

    // Try to extract the error message from python
    std::string errorMessage = "Undefined Error";
    try{
      PyObject *exc,*val,*tb;
      PyErr_Fetch(&exc,&val,&tb);
      PyErr_NormalizeException(&exc,&val,&tb);
      boost::python::handle<> hexc(exc),hval(boost::python::allow_null(val)),htb(boost::python::allow_null(tb));
      if(!hval)
      {
        errorMessage = boost::python::extract<std::string>(boost::python::str(hexc));
      }
      else
      {
        // Get Error message
        boost::python::object traceback(boost::python::import("traceback"));
        boost::python::object format_exception(traceback.attr("format_exception"));
        boost::python::object formatted_list(format_exception(hexc,hval,htb));
        boost::python::object formatted(boost::python::str("").join(formatted_list));

        errorMessage = boost::python::extract<std::string>(formatted);

        // Cutoff empty line at the end
        if(errorMessage.at(errorMessage.length() - 1) == '\n') errorMessage = errorMessage.substr(0, errorMessage.length() - 1);

        // Extract line number
        std::string substring = errorMessage.substr(errorMessage.find("line") + 4, errorMessage.length());
        std::stringstream stream(substring);
        long lineno;
        stream >> lineno;
        if(stream) emit errorMessageLineNumberChanged(lineno);
      }
    }
    catch(const std::exception& e)
    {
      errorMessage = e.what();
    }
    catch(const boost::python::error_already_set&)
    {
      errorMessage = "Undefined error";
    }

    // Post the error in the Log section of cedar
    cedar::aux::LogSingleton::getInstance()->error
      (
        errorMessage,
        "void cedar::proc::steps::PythonScript::executePythonScript()",
        this->getName()
      );

  }

  if(cedar::proc::steps::PythonScript::executionFailed) this->setState(cedar::proc::Triggerable::STATE_EXCEPTION, "An exception occured");
  else this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");

  freePythonVariables();

  this->mIsExecuting = 0;

  mutex.unlock();
  mWasResetted= false;
}

void cedar::proc::steps::PythonScript::inputConnectionChanged(const std::string& inputName)
{
  // Put input in the corresponding field in mInputs
  cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(getInput(inputName));

  unsigned slot_id;
  std::istringstream(inputName.substr(5)) >> slot_id;


  if (slot_id < mInputs.size())
  {
    mInputs[slot_id] = input;
  }
}

void cedar::proc::steps::PythonScript::compute(const cedar::proc::Arguments&)
{
  if(!this->mIsExecuting)
  {
    executePythonScript(false);
  }
}

// Method is called when the executeButton in the CodeWidget is clicked
void cedar::proc::steps::PythonScript::executeButtonClicked(){
  if(!this->mIsExecuting)
  {
    executePythonScript(true);
  }
}

void cedar::proc::steps::PythonScript::reset()
{
  mStates.clear();
  mWasResetted= true;
}

#endif // CEDAR_USE_PYTHONSTEP