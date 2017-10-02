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

    File:        Boost.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 08 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_BOOST_H
#define CEDAR_PROC_SOURCES_BOOST_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/sources/Boost.fwd.h"

// SYSTEM INCLUDES


/*!@brief A uniform activation boost.
 */
class cedar::proc::sources::Boost : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Boost();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief convenience function for accessing the strength of the boost
  inline double getStrength() const
  {
    return this->_mStrength->getValue();
  }

  //!@brief convenience funtion to set the boost strength
  inline void setStrength(double strength)
  {
    return this->_mStrength->setValue(strength);
  }

  //!@brief convenience function for accessing the activity flag
  inline bool isActive() const
  {
    return this->_mActive->getValue();
  }

  //!@brief Sets whether the boost is active.
  inline void setActive(bool active)
  {
    this->_mActive->setValue(active);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments& arguments);

  void reset();

private slots:
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The output matrix containing the boost.
  cedar::aux::MatDataPtr mBoost;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! The strength of the boost.
  cedar::aux::DoubleParameterPtr _mStrength;

  //! Whether or not the boost is active.
  cedar::aux::BoolParameterPtr _mActive;

  //! Whether or not the boost is deactivated when a reset occurs.
  cedar::aux::BoolParameterPtr _mDeactivateOnReset;
}; // class cedar::proc::sources::Boost

#endif // CEDAR_PROC_SOURCES_BOOST_H

