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
#include "cedar/processing/sources/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES

/*!@brief The base class for all grabber sources for the processingIde
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
  GrabberBase();
  //!@brief The standard constructor.

public:
  //!@brief Destructor
  virtual ~GrabberBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void saveSnapshot();

public slots:
  void setRecording();

  //!@todo Enum RecordType (Encoding)

  //!@brief Sets a new configuration filename
  void setConfigurationFileName();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //!@brief Invoke this function in the derived class
  //  invokes onCreateGrabber() form derived class and updates information from grabber for the gui-parameter
  void createGrabber();

  //!@brief Create the grabber in the derived class
  //  apply the new created Grabber to GrabberBase::mGrabber
  virtual void onCreateGrabber() = 0;

  //!@brief Applies an appropriate annotation to the current image.
  void annotateImage();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The used Grabber stored in this pointer
  cedar::dev::sensors::visual::GrabberPtr mGrabber;

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
  cedar::aux::FileParameterPtr mRecordName;
  //cedar::aux::BoolParameterPtr mSaveSnapshot;
  cedar::aux::FileParameterPtr mSnapshotName;

  //!@brief The configuration filename
  cedar::aux::FileParameterPtr _mConfigurationFileName;
  //!@todo Enum RecordType (Encoding)

  //!@endcond
private:
  // none yet

}; //class cedar::proc::sources::GrabberBase

#endif // CEDAR_PROC_SOURCES_GRABBER_SOURCE_H

