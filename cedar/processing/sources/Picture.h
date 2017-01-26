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
#include "cedar/processing/sources/GrabberBase.h"
#include "cedar/processing/Step.h"
#include "cedar/devices/sensors/visual/PictureGrabber.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/Picture.fwd.h"

// SYSTEM INCLUDES


//!@brief A picture file source for the processing framework.
class cedar::proc::sources::Picture
:
public cedar::proc::sources::GrabberBase
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
  ~Picture();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Loads the given file into the picture source.
  void setSourceFileName(const std::string& imagePath);

  //! Returns the mat data pointer containing the current picture.
  cedar::aux::MatDataPtr getImage();

  //! Returns the mat data pointer containing the current picture.
  cedar::aux::ConstMatDataPtr getImage() const;

public slots:
  //!@brief a slot that takes care of updating the output picture
  void updatePicture();

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
  void reset();

  //!@brief Cast the base GrabberBasePtr to derived class PictureGrabberPtr
  inline cedar::dev::sensors::visual::PictureGrabberPtr getPictureGrabber()
  {
    return boost::static_pointer_cast<cedar::dev::sensors::visual::PictureGrabber>
           (
             this->cedar::proc::sources::Picture::mpGrabber
           );
  }

  //!@brief Cast the base GrabberBasePtr to derived class PictureGrabberPtr
  inline cedar::dev::sensors::visual::ConstPictureGrabberPtr getPictureGrabber() const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::visual::PictureGrabber>
           (
             this->cedar::proc::sources::Picture::mpGrabber
           );
  }


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

}; // class cedar::proc::sources::Picture

#endif // CEDAR_PROC_SOURCES_PICTURE_H


