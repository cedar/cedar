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

    File:        Annotatable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_ANNOTATABLE_H
#define CEDAR_AUX_ANNOTATION_ANNOTATABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/namespace.h"
#include "cedar/auxiliaries/annotation/Annotation.h"

// SYSTEM INCLUDES


/*!@brief Base class for anything that can have annotations.
 */
class cedar::aux::annotation::Annotatable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef std::vector<cedar::aux::annotation::AnnotationPtr> AnnotationList;
  typedef AnnotationList::iterator AnnotationIterator;
  typedef AnnotationList::const_iterator AnnotationConstIterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Destructor
  virtual ~Annotatable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets an annotation. If there is already an annotation of the exact type of the given annotation, it is replaced.
  void setAnnotation(cedar::aux::annotation::AnnotationPtr annotation);

  /*! Returns an annotation of the given type.
   *
   *  @throws A cedar::aux::UnknownTypeException if no matching annotation can be found.
   */
  template <typename T>
  boost::shared_ptr<T> getAnnotation()
  {
    return cedar::aux::asserted_pointer_cast<T>(this->mAnnotations[this->findAnnotation<T>()]);
  }

  /*! Returns an annotation of the given type as a constant pointer.
   *
   *  @throws A cedar::aux::UnknownTypeException if no matching annotation can be found.
   */
  template <typename T>
  boost::shared_ptr<const T> getAnnotation() const
  {
    return cedar::aux::asserted_pointer_cast<const T>(this->mAnnotations[this->findAnnotation<T>()]);
  }

  //! Copies all annotations from the given data pointer.
  void copyAnnotationsFrom(cedar::aux::annotation::ConstAnnotatablePtr other);

  /*! @brief  Returns a description for the annotatable.
   *
   *          Built from the description of all the annotations in this object.
   */
  virtual std::string getDescription() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Finds the index an annotation based on a type.
  template <typename T>
  size_t findAnnotation() const
  {
    for (size_t i = 0; i < this->mAnnotations.size(); ++i)
    {
      if (typeid(*this->mAnnotations[i]) == typeid(T))
      {
        return i;
      }
    }

    CEDAR_THROW(cedar::aux::AnnotationNotFoundException, "Could not find an annotation of the given type.");
  }

  //!@brief Finds the index an annotation based on the type of a given object.
  size_t findAnnotation(cedar::aux::annotation::AnnotationPtr annotation) const
  {
    for (size_t i = 0; i < this->mAnnotations.size(); ++i)
    {
      if (typeid(*this->mAnnotations[i]) == typeid(*annotation))
      {
        return i;
      }
    }

    CEDAR_THROW(cedar::aux::AnnotationNotFoundException, "Could not find an annotation of the given type.");
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! List of annotations.
  AnnotationList mAnnotations;

}; // class cedar::aux::annotation::Annotatable

#endif // CEDAR_AUX_ANNOTATION_ANNOTATABLE_H

