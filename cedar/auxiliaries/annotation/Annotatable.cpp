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

    File:        Annotatable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/Annotatable.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::annotation::Annotatable::~Annotatable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::annotation::Annotatable::setAnnotation(cedar::aux::annotation::AnnotationPtr annotation)
{
  if (this->hasAnnotation(annotation))
  {
    size_t i = this->findAnnotation(annotation);
    this->mAnnotations[i] = annotation;
  }
  else
  {
    this->mAnnotations.push_back(annotation);
  }
}

void cedar::aux::annotation::Annotatable::copyAnnotationsFrom(cedar::aux::annotation::ConstAnnotatablePtr other)
{
  this->mAnnotations.clear();
  for (size_t i = 0; i < other->mAnnotations.size(); ++i)
  {
    if (!other->mAnnotations[i]->excludeFromCopying())
    {
      this->mAnnotations.push_back(other->mAnnotations[i]->clone());
    }
  }
}

std::string cedar::aux::annotation::Annotatable::getDescription() const
{
  std::string description;
  std::string clean_type = cedar::aux::objectTypeToString(this);
  clean_type = cedar::aux::replace(clean_type, "<", "&lt;");
  clean_type = cedar::aux::replace(clean_type, ">", "&gt;");
  description += "Implementation: <tt>" + clean_type + "</tt>";

  for (size_t i = 0; i < this->mAnnotations.size(); ++i)
  {
    description += "<hr />";
    description += this->mAnnotations[i]->getDescription();
  }

  return description;
}
