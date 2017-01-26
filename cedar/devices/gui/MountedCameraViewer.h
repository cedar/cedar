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

    File:        MountedCameraViewer.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 06 15

    Description: viewer simulating a camera that is fixed to a kinematic chain

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GL_MOUNTED_CAMERA_VIEWER_H
#define CEDAR_DEV_GL_MOUNTED_CAMERA_VIEWER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/devices/gui/namespace.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES

/*!@brief A widget for displaying a mounted camera.
 */
class cedar::dev::gui::MountedCameraViewer
:
public cedar::aux::gui::Viewer,
public cedar::aux::NamedConfigurable
{
private:
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief the constructor */
  MountedCameraViewer
  (
    cedar::aux::gl::ScenePtr pScene,
    const cedar::dev::KinematicChainPtr chain,
    bool readFromFile = true
  );

  /*!@brief the constructor */
  ~MountedCameraViewer();

  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief override to remove the mouse handling*/
  virtual void mousePressEvent(QMouseEvent* e);
  /*!@brief override to remove the mouse handling*/
  virtual void wheelEvent(QWheelEvent* e);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief draws all objects in the scene */
  void draw();

  /*!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
   *
   * @param node json node providing the configuration
   */
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  cedar::dev::KinematicChainPtr mArm;
  cedar::aux::LocalCoordinateFramePtr mCameraCoordinateFrame;
  cedar::aux::IntParameterPtr _mMountingJoint;
  cedar::aux::IntParameterPtr _mImageWidth;
  cedar::aux::IntParameterPtr _mImageHeight;
  cedar::aux::DoubleVectorParameterPtr _mCalibrationMatrix;
  //! transformations to the camera frame (assuming reference configuration)
  cv::Mat mReferenceCameraTransformation;
  cv::Mat mTransformationTranspose;
  cv::Mat mCalibrationMatrix;
  cv::Mat mProjection;
};
#endif  // CEDAR_DEV_GL_MOUNTED_CAMERA_VIEWER_H
