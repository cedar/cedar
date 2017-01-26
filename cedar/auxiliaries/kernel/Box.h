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

    File:        Box.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 12

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_BOX_H
#define CEDAR_AUX_KERNEL_BOX_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/Box.fwd.h"

// SYSTEM INCLUDES


/*!@brief Gauss kernel class.
 *
 */
class cedar::aux::kernel::Box : public cedar::aux::kernel::Separable
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
  Box();

  //!@brief The destructor.
  ~Box();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief convenience function for getting the amplitude of the kernel
  inline double getAmplitude() const
  {
    return this->_mAmplitude->getValue();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected slots:
  //!@brief update the dimensionality of the kernel matrices, triggered by a signal (e.g. a changed parameter value)
  void updateDimensionality();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Updates the box kernel's matrix.
   */
  void calculateParts();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The amplitude of the kernel.
  cedar::aux::DoubleParameterPtr _mAmplitude;

  //!@brief The widths of the box kernel for each dimension.
  cedar::aux::UIntVectorParameterPtr _mWidths;

  //!@brief The shift of the box in each dimension.
//  cedar::aux::IntVectorParameterPtr _mShifts;

private:
  // none yet

}; // class cedar::aux::kernel::Box

#endif // CEDAR_AUX_KERNEL_BOX_H
