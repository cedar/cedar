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

    File:        boostSignalsHelper.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 19

    Description: Defines a set of macros that are intended to ease the use of boost signals/slots.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_BOOST_SIGNALS_HELPER_H
#define CEDAR_AUX_BOOST_SIGNALS_HELPER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif


#define CEDAR_SIGNAL_MEMBER_NAME(SIGNAL_NAME) \
  signal ## SIGNAL_NAME

// Declares a signal inside a class.
#define CEDAR_DECLARE_SIGNAL(SIGNAL_NAME, SIGNATURE) \
    inline boost::signals2::connection connectTo ## SIGNAL_NAME ## Signal(const boost::function<SIGNATURE>& function) const \
    { \
      return this->CEDAR_SIGNAL_MEMBER_NAME(SIGNAL_NAME).connect(function); \
    } \
  private: \
    mutable boost::signals2::signal<SIGNATURE> CEDAR_SIGNAL_MEMBER_NAME(SIGNAL_NAME);


#endif // CEDAR_AUX_CASTS_H
