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

    File:        Depth.h

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.ruhr-uni-bochum.de
    Date:        2013 05 03

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_DEPTH_H
#define CEDAR_AUX_ANNOTATION_DEPTH_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/Annotation.h"
#include "cedar/auxiliaries/Cloneable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/Depth.fwd.h"

// SYSTEM INCLUDES
#include <string>

/*!@brief An annotation that indicates depth data.
 */
class cedar::aux::annotation::Depth
:
public cedar::aux::annotation::Annotation,
public cedar::aux::Cloneable<cedar::aux::annotation::Depth, cedar::aux::annotation::Annotation>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor.
  Depth();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getDescription() const;

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

}; // class cedar::aux::annotation::Depth

#endif // CEDAR_AUX_ANNOTATION_DEPTH_H

