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

    File:        SizesRangeHint.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 07 24

    Description: Header file for the class cedar::aux::annotation::SizesRangeHint.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_SIZES_RANGE_HINT_H
#define CEDAR_AUX_ANNOTATION_SIZES_RANGE_HINT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/Annotation.h"
#include "cedar/auxiliaries/math/Limits.h"
#include "cedar/auxiliaries/Cloneable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/SizesRangeHint.fwd.h"

// SYSTEM INCLUDES


/*!@brief An annotation that hints at the range of dimensions
 */
class cedar::aux::annotation::SizesRangeHint
:
public cedar::aux::annotation::Annotation,
public cedar::aux::Cloneable<cedar::aux::annotation::SizesRangeHint, cedar::aux::annotation::Annotation>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SizesRangeHint(const std::vector<cedar::aux::math::Limits<double>>& limits);

  //!@brief Destructor
  virtual ~SizesRangeHint();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
    std::string getDescription() const;

    const std::vector<cedar::aux::math::Limits<double>>& getRange() const;

    bool excludeFromCopying() const;

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
    std::vector<cedar::aux::math::Limits<double>> mRange;

}; // class cedar::aux::annotation::SizesRangeHint

#endif // CEDAR_AUX_ANNOTATION_SIZES_RANGE_HINT_H

