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

    File:        KernelList.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 13

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_CONV_KERNEL_LIST_H
#define CEDAR_AUX_CONV_KERNEL_LIST_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/Kernel.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/convolution/KernelList.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <vector>
#include <QObject>

/*!@brief This is a structure for storing a list of kernels.
 */
class cedar::aux::conv::KernelList : public QObject
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  KernelList();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief append a new kernel at the back of the list
  void append(cedar::aux::kernel::KernelPtr kernel);

  //!@brief remove a kernel at a given index
  void remove(size_t index);

  //!@brief insert a kernel at a given index
  inline void setKernel(size_t index, cedar::aux::kernel::KernelPtr kernel)
  {
    this->mKernels.at(index) = kernel;

    this->mKernelChangedSignal(index);
  }

  //!@brief resizes the list
  inline void resize(size_t size)
  {
    this->mKernels.resize(size);
  }

  //!@brief access a kernel at given index
  inline cedar::aux::kernel::ConstKernelPtr getKernel(size_t i) const
  {
    return this->mKernels.at(i);
  }

  //!@brief access a kernel at given index
  inline cedar::aux::kernel::KernelPtr getKernel(size_t i)
  {
    return this->mKernels.at(i);
  }

  //!@brief returns the size of kernels in this list
  inline size_t size() const
  {
    return this->mKernels.size();
  }

  //!@brief removes all kernels from the list
  inline void clear()
  {
    if (this->mKernels.size() > 0)
    {
      // have to do this in reverse order
      for (size_t i = this->mKernels.size() - 1; i > 0; --i)
      {
        this->mKernelRemovedSignal(i);
      }
      this->mKernelRemovedSignal(0);
    }
    this->mKernels.clear();
  }

  //!@brief process adding a new kernel
  inline boost::signals2::connection connectToKernelAddedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelAddedSignal.connect(slot);
  }

  //!@brief process changes in a kernel
  inline boost::signals2::connection connectToKernelChangedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelChangedSignal.connect(slot);
  }

  //!@brief process removal of a kernel
  inline boost::signals2::connection connectToKernelRemovedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelRemovedSignal.connect(slot);
  }

  //! this function calculates and returns the combined kernel of all kernels included in this list
  cv::Mat getCombinedKernel() const;

  //!@brief returns true if all kernels in list have same dimensionality and size
  bool checkForSameKernelSize() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
signals:
  //! Emitted, when the combined kernel is updated.
  void combinedKernelUpdated();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

private slots:
  void calculateCombinedKernel();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! A vector of all the kernels in this list.
  std::vector<cedar::aux::kernel::KernelPtr> mKernels;

  boost::signals2::signal<void (size_t)> mKernelAddedSignal;

  boost::signals2::signal<void (size_t)> mKernelChangedSignal;

  boost::signals2::signal<void (size_t)> mKernelRemovedSignal;

  cedar::aux::MatDataPtr mCombinedKernel;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::conv::KernelList

#endif // CEDAR_AUX_CONV_KERNEL_LIST_H

