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

    File:        VirtualCamera.h

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 05 12

    Description: Header file for the class cedar::proc::sources::VirtualCamera.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_VIRTUAL_CAMERA_H
#define CEDAR_PROC_SOURCES_VIRTUAL_CAMERA_H

// CEDAR CONFIGURATION
#include <cedar/auxiliaries/DoubleVectorParameter.h>
#include "cedar/configuration.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/VirtualCamera.fwd.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#ifdef CEDAR_USE_QGLVIEWER
   #include "cedar/auxiliaries/gui/Viewer.h"
#endif CEDAR_USE_QGlVIEWER
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

#ifdef CEDAR_USE_QGLVIEWER



class cedar::proc::sources::VirtualCamera : public cedar::proc::Step
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
  VirtualCamera();

  //!@brief Destructor
  virtual ~VirtualCamera();

public slots:
          //!@brief slots for changing camera parameters
  void resolutionChanged();
  void cameraPositionChanged();
  void cameraOrientationChanged();
  void cameraPositionChangedFromViewport();
  void cameraOrientationChangedFromViewport();
  void updatedView();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@note This method isn't called !
   *       A temporary fix is in place which updates the output via a signal emmited from mpViewer
   */
  void compute(const cedar::proc::Arguments&);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // output
  cedar::aux::MatDataPtr mpOutput;

private:
  //cedar::aux::gui::ViewerWeakPtr mpViewer;
#ifdef CEDAR_USE_QGLVIEWER
  boost::shared_ptr<cedar::aux::gui::Viewer> mpViewer;
#endif CEDAR_USE_QGlVIEWER
  //!@brief sizes of all dimensions of the output of the projection
  cedar::aux::UIntVectorParameterPtr mOutputSizes;
  //!@brief position of the virtual camera (x,y,z)
  cedar::aux::DoubleVectorParameterPtr mCameraPosition;
  //!@brief orientation of the virtual camera
  cedar::aux::DoubleVectorParameterPtr mHorizontalOrientation;
  cedar::aux::DoubleVectorParameterPtr mVerticalOrientation;

  QReadWriteLock* mLock;

}; // class cedar::proc::sources::VirtualCamera

#endif // CEDAR_USE_QGLVIEWER
#endif // CEDAR_PROC_SOURCES_VIRTUAL_CAMERA_H

