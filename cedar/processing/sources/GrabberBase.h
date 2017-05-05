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

    File:        GrabberBase.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 05 23

    Description: The header for the GrabberBase class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_GRABBER_SOURCE_H
#define CEDAR_PROC_SOURCES_GRABBER_SOURCE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/FileParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/GrabberBase.fwd.h"

// SYSTEM INCLUDES

/*!@brief The base class for all grabber sources for the processing framework.
 *
 *    This class implements the common structure of all grabber sources
 */
class cedar::proc::sources::GrabberBase
:
public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  GrabberBase(bool looped = true);

public:
  //!@brief Destructor
  virtual ~GrabberBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Save a Snapshot of the current picture
   *
   * Implemented as an action in the grabber-step of processingGUI
   */
  void saveSnapshot();

public slots:
  //!@brief Slot for the recording-checkbox
  void setRecording();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //!@brief Applies an appropriate annotation to the current image.
  void annotateImage();
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief Cast the base GrabberBasePtr to derived class PictureGrabberPtr
  inline cedar::dev::sensors::visual::GrabberPtr getGrabber()
  {
    return boost::static_pointer_cast<cedar::dev::sensors::visual::Grabber>
           (
             this->cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //!@brief Cast the base GrabberBasePtr to derived class PictureGrabberPtr
  inline cedar::dev::sensors::visual::ConstGrabberPtr getGrabber() const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::visual::Grabber>
           (
             this->cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The used Grabber stored in this pointer
  cedar::dev::sensors::visual::GrabberPtr mpGrabber;

  //!@brief The grabbed Image
  cedar::aux::MatDataPtr mImage;


private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@cond SKIPPED_DOCUMENTATION
  // the values of the properties
  cedar::aux::BoolParameterPtr mRecording;

  //!@todo Add an enum class for the encoding type used when recording

  //!@endcond
private:
  // none yet

}; //class cedar::proc::sources::GrabberBase

#endif // CEDAR_PROC_SOURCES_GRABBER_SOURCE_H

