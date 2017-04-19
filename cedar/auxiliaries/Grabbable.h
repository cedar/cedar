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

    File:        Grabbable.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 05 15

    Description: The grabbable interface class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GRABBABLE_H
#define CEDAR_AUX_GRABBABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Grabbable.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>


/*!@brief An interface class to describe an interface to grab from any other class
 *
 * Implement this interface in your class, if the InterfaceGrabber should grab from it
 *
 */
class cedar::aux::Grabbable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  Grabbable()
  {
  };

public:

  //!@brief Destructor
  virtual ~Grabbable()
  {
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Grab the Image in the class
   *
   * @return The image in a cv::Mat structure
   */
  virtual const cv::Mat& grabImage() = 0;


  /*!@brief initialize the grabber specific parts in this method.
   *
   * The QReadWriteLock should be managed by the derived class.
   * The grabber invokes this method in it's constructor.
   * Have a look at the class cedar::aux::gui::Viewer for an implementation
   *
   * @return returns the lock for the image-mat, if there isn't already a grabber connected.
   *         Otherwise it will return NULL
   */
  virtual QReadWriteLock* registerGrabber() = 0;


  /*!@brief deinitialize the grabber specific parts in this method.
   *
   * The grabber invokes this method in it's destructor.
   * Have a look at the class cedar::aux::gui::Viewer for an implementation
   *
   * @param lock This is the lock which comes from the registerGrabber() method. This parameter is used to
   *    check, if the assigned grabber invokes the deregisterGrabber() method.
   *    After invocation of this method, lock should be set to NULL!
   */
  virtual void deregisterGrabber(QReadWriteLock* lock) = 0;

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
  // none yet

}; // class cedar::aux::Grabbable

#endif // CEDAR_AUX_GRABBABLE_H

