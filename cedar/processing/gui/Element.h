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

    File:        Element.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 01 23

    Description: Header file for the class cedar::proc::gui::Element.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ELEMENT_H
#define CEDAR_PROC_GUI_ELEMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/Element.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::gui::Element : public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Element
  (
    qreal width,
    qreal height,
    cedar::proc::gui::GraphicsBase::GraphicsGroup group,
    cedar::proc::gui::GraphicsBase::GraphicsGroup canConnectTo = GRAPHICS_GROUP_NONE,
    cedar::proc::gui::GraphicsBase::BaseShape shape = BASE_SHAPE_ROUNDED_RECT
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief   Returns the element associated with this graphics item.
   *
   * @remarks In cases where this item is not associated with an element, this may return an empty pointer!
   */
  inline cedar::proc::ConstElementPtr getElement() const
  {
    return this->mElement;
  }

  /*!@brief   Returns the element associated with this graphics item.
   *
   * @remarks In cases where this item is not associated with an element, this may return an empty pointer!
   */
  inline cedar::proc::ElementPtr getElement()
  {
    return this->mElement;
  }

  /*!@brief   Deletes the underlying element.
   */
  virtual void deleteElement();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Sets the element associated with this graphics item.
   */
  inline void setElement(cedar::proc::ElementPtr element)
  {
    this->mElement = element;
  }

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
  //!@brief The element associated with this graphics item
  cedar::proc::ElementPtr mElement;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::Element

#endif // CEDAR_PROC_GUI_ELEMENT_H

