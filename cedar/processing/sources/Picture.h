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

    File:        Picture.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_PICTURE_H
#define CEDAR_PROC_SOURCES_PICTURE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/PictureGrabber.h"
#include "cedar/auxiliaries/ImageData.h"
#include "cedar/auxiliaries/FileParameter.h"

// SYSTEM INCLUDES


//!@brief A picture file source for the processingIde

class cedar::proc::sources::Picture : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Picture();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:

  //!@brief A slot that is triggered if a new filename should be set
  void setFileName();

  //!@brief Sets a new configuration filename
  void setConfigurationFileName();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);
  void onStart();

  //create a new grabber instance
  void createGrabber();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ImageDataPtr mImage;

  cedar::dev::sensors::visual::PictureGrabberPtr mGrabber;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief the filename to grab from
  cedar::aux::FileParameterPtr _mFileName;

  //!@brief The configuration filename
  cedar::aux::FileParameterPtr _mConfigurationFileName;

}; // class cedar::proc::sources::Picture

#endif // CEDAR_PROC_SOURCES_PICTURE_H


