/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Kernel.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_KERNEL_H
#define CEDAR_AUX_KERNEL_KERNEL_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/processing/namespace.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>
#include <QObject>

/*!@brief Meta class to derive from when implementing kernels.
 *
 * This is the kernel meta class that provides a uniform interface for
 * all implemented kernels. This class already provides the functionality of
 * saving and loading a kernel matrix to and from a file. These two functions
 * loadKernelFromFile() and saveKernelToFile() are declared virtual to allow for derived classes
 * to implement their own save routine. This class also provides a simple implementation of a get
 * function to get a reference to the internal kernel matrix. Without giving back a reference,
 * the return value would be a copy of the internal matrix and therefore would not be affected
 * by changes of the kernel configuration. The last provided function getReadWriteLock() returns a QReadWriteLock
 * Kernel::mpReadWriteLockOutput, which protects the kernel data from being accessed while being updated.
 *
 * There is a set of purely virtual functions, which have to be implemented by all derived classes.
 * The first one is doLocalInit(). This should contain all code that is necessary to create a new instance
 * of a derived kernel. The second one is calculate(), which should contain all the code necessary to
 * calculate an updated version of the kernel matrix once parameters of the kernel have changed.
 *
 */
class cedar::aux::kernel::Kernel : public QObject, public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Kernel();
  /*!@brief Constructor with some parameters.
   * @param dimensionality the dimensionality of a kernel
   * @param kernelFile a file from which to load and to which to store the kernel matrix
   */
  Kernel(unsigned int dimensionality);
  //!@brief Destructor
  virtual ~Kernel();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! virtual function that in the derived classes actually does the job of initializing
   * the kernel from file
   */
  //!@todo deal with boost PropertyTree here
  virtual void onInit(){};

  /*! virtual function for accessing the kernel matrix, in the 1d case this is the default
   * function to use, in the 2d case this function should return the combined 2d kernel
   * @return the kernel matrix
   */
  virtual const cv::Mat& getKernel() const;
  //!@brief get the internal DataPtr
  const cedar::aux::DataPtr getKernelRaw() const;

  //!@brief Calculates the convolution of the kernel and the matrix.
  virtual cv::Mat convolveWith(const cv::Mat& mat) const;

  /*!@brief get access to the write lock when in an asynchronous mode
   * @return pointer to the QReadWriteLock
   */
  QReadWriteLock* getReadWriteLock();

  /*!@brief get the dimensionality of the kernel matrix
   * @return dimensionality
   */
  unsigned int getDimensionality() const;

  /*!@brief sets the dimensionality of the kernel matrix
   * @param dimensionality the new desired dimensionality
   */
  void setDimensionality(unsigned int dimensionality);

  /*!@brief Hides the dimensionality parameter.
   */
  void hideDimensionality(bool hide);

public slots:
  //!@todo merge update kernel and calculate, if calculate can be set to public and pure virtual works with Qt..
  void updateKernel();

signals:
  //!@brief signal that is emitted once the internal matrices have been updated (e.g. for plotting widgets)
  void kernelUpdated();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief virtual function to calculate the kernel matrix
   */
  virtual void calculate() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief matrix containing the kernel
  cedar::aux::MatDataPtr mKernel;
  //!@brief read and write lock to protect the kernel when calculating its values
  QReadWriteLock *mpReadWriteLockOutput;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief dimensionality of this kernel
  cedar::aux::UIntParameterPtr _mDimensionality;
  //!@todo put this to a separate kernel implementation
  // a file from which the kernel matrix can be loaded
  //cedar::aux::StringParameterPtr _mKernelMatrixFile;

private:
  // none yet

}; // class cedar::aux::kernel::Kernel

#endif // CEDAR_AUX_KERNEL_KERNEL_H
