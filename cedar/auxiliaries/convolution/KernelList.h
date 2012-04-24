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
#include "cedar/auxiliaries/convolution/namespace.h"
#include "cedar/auxiliaries/kernel/namespace.h"

// SYSTEM INCLUDES
#include <boost/signals2.hpp>
#include <vector>

/*!@brief This is a structure for storing a list of kernels.
 */
class cedar::aux::conv::KernelList
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  void append(cedar::aux::kernel::KernelPtr kernel);

  void remove(size_t index);

  inline void setKernel(size_t index, cedar::aux::kernel::KernelPtr kernel)
  {
    this->mKernels.at(index) = kernel;

    this->mKernelChangedSignal(index);
  }

  inline void resize(size_t size)
  {
    this->mKernels.resize(size);
  }

  inline cedar::aux::kernel::ConstKernelPtr getKernel(size_t i) const
  {
    return this->mKernels.at(i);
  }

  inline size_t size() const
  {
    return this->mKernels.size();
  }

  inline void clear()
  {
    this->mKernels.clear();
  }

  inline boost::signals2::connection connectToKernelAddedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelAddedSignal.connect(slot);
  }

  inline boost::signals2::connection connectToKernelChangedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelChangedSignal.connect(slot);
  }

  inline boost::signals2::connection connectToKernelRemovedSignal(boost::function<void (size_t)> slot)
  {
    return this->mKernelRemovedSignal.connect(slot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::conv::KernelList

#endif // CEDAR_AUX_CONV_KERNEL_LIST_H

