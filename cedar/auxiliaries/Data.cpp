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

    File:        Data.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Data::Data()
:
mpLock(new QReadWriteLock()),
mpeOwner(NULL)
{
}

cedar::aux::Data::~Data()
{
  delete mpLock;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Data::setAnnotation(cedar::aux::annotation::AnnotationPtr annotation)
{
  try
  {
    AnnotationIterator iter = this->findAnnotation(annotation);
    *iter = annotation;
  }
  catch (cedar::aux::AnnotationNotFoundException&)
  {
    this->mAnnotations.push_back(annotation);
  }
}

void cedar::aux::Data::copyAnnotationsFrom(cedar::aux::ConstDataPtr other)
{
  this->mAnnotations.resize(other->mAnnotations.size());
  for (size_t i = 0; i < other->mAnnotations.size(); ++i)
  {
    this->mAnnotations[i] = other->mAnnotations[i]->clone();
  }
}

QReadWriteLock& cedar::aux::Data::getLock()
{
  return *this->mpLock;
}

QReadWriteLock& cedar::aux::Data::getLock() const
{
  return *this->mpLock;
}

cedar::aux::Configurable* cedar::aux::Data::getOwner() const
{
  return this->mpeOwner;
}

void cedar::aux::Data::setOwner(cedar::aux::Configurable* step)
{
  this->mpeOwner = step;
}
