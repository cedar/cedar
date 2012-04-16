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

    File:        InterfaceData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 03 02

    Description: Header file for the \em IntervalData class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_INTERVAL_DATA_H
#define CEDAR_AUX_INTERVAL_DATA_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UserData.h"

// SYSTEM INCLUDES


/*!
 * @brief Class for interval data mainly used by automated gui creation.
 */
template <typename T>
class cedar::aux::IntervalData : public cedar::aux::UserData
{
  public:
    /*!@brief The default constructor of IntervalData.
     *
     * @param min    the left (included) border of the interval
     * @param max    the right (included) border of the interval
     * @param step    the step size of this interval (e.g. usable by SpinBoxes)
     * @param isReadOnly    represented interval is read-only (doesn't affect a member)
     */
    IntervalData(T min = 0, T max = 255, T step = 1, bool isReadOnly = false)
    {
      mMin = min;
      mMax = max;
      mStep = step;
      mIsReadOnly = isReadOnly;
    }

    bool mIsReadOnly; //!< Whether or not the parameter can be changed in the user interface.

    T mMin; //!< The minimum value for the user interface.
    T mMax; //!< The maximum value for the user interface.
    T mStep; //!< The step size for the user interface.
};

#endif // CEDAR_AUX_INTERVAL_DATA_H
