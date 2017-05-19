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

    File:        Annotation.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_ANNOTATION_H
#define CEDAR_AUX_ANNOTATION_ANNOTATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Cloneable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/Annotation.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Base class for annotations.
 *
 *        Annotations can be used to give meaning to objects that are cedar::aux::annotation::Annotatable. This adds the
 *        possiblity to add semantic meaning at run-time, providing more flexibility than classes and inheritance.
 *
 *        Examples for the use of annotations include: matrix data can be annotated with a
 *        cedar::aux::annotation::ColorSpace to indicate that it contains an image rather than a standard matrix. This
 *        can then be interpreted, e.g., by the plotting system and let it know to automatically open an image plot.
 */
class cedar::aux::annotation::Annotation : public virtual cedar::aux::CloneableBase<cedar::aux::annotation::Annotation>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Annotation();

  //!@brief The copy constructor.
  Annotation(const Annotation& copyFrom);

  //!@brief Destructor
  virtual ~Annotation();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief   Implement this to return a description string for the given annotation.
   *
   * @remarks This string should contain HTML, e.g., for bold faced fonts and italics, as well as &lt;br /&gt; for line
   *          breaks.
   */
  virtual std::string getDescription() const = 0;

  /*! If this function returns true, Annotatable will not copy this annotation in the copyAnnotationsFrom function.
   *
   * The default implementation returns false.
   */
  virtual bool excludeFromCopying() const;

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

}; // class cedar::aux::annotation::Annotation

#endif // CEDAR_AUX_ANNOTATION_ANNOTATION_H

