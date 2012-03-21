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

    File:        LocalCoordinateFrame.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: header of cedar::aux::LocalCoordinateFrame class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOCAL_COORDINATE_FRAME_H
#define CEDAR_AUX_LOCAL_COORDINATE_FRAME_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>

/*!@brief Provides the geometry of a rigid object
 *
 * rigid between world coordinate frame and object coordinate frame
 */
class cedar::aux::LocalCoordinateFrame : public QObject, public cedar::aux::Configurable
{
private:
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LocalCoordinateFrame();

  //!@brief Destructor
  virtual ~LocalCoordinateFrame();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief read a configuration for all registered parameters from a cedar::aux::ConfigurationNode
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief returns position of the object in homogeneous coordinates
  cv::Mat getPosition() const;

  //!@brief returns x-position of the object frame origin in world frame
  double getPositionX() const;

  //!@brief returns y-position of the object frame origin in world frame
  double getPositionY() const;

  //!@brief returns z-position of the object frame origin in world frame
  double getPositionZ() const;

  //!@brief returns the \f$4 \times 4\f$ rigid transformation matrix of the object frame relative to the world frame
  cv::Mat getTransformation() const;

  /*!@brief sets the transformation matrix
   * @param transformation \f$4 \times 4\f$ rigid transformation matrix of the object frame relative to the world frame
   */
  void setTransformation(cv::Mat transformation);

public slots:
  //!@brief updates the model
  virtual void update();

  /*!@brief set the position of the object frame origin in the world frame
   * @param x    coordinates of
   * @param y    value for green channel in RGB color
   * @param z    value for blue channel in RGB color
   */
  void setPosition(double x, double y, double z);

  /*!@brief set the position of the object frame origin in the world frame
   * @param position    new position in homogeneous coordinates
   */
  void setPosition(const cv::Mat& position);

  /*!@brief set the position of the object frame origin in the world frame
   * @param position    new position in homogeneous coordinates
   */
  void setPosition(const std::vector<double>& position);

  /*!@brief rotates the object around one of the main axes of the object frame
   * @param axis    index of the axis to rotate around, between 0 and 2
   * @param angle    value of angle that the object is rotated by
   */
  void rotate(unsigned int axis, double angle);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialization function
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:
  //!@brief rigid transformation to the object
  cv::Mat mTransformation;

  std::vector<cv::Mat> mUnitAxes;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! lock for thread safety
  mutable QReadWriteLock mLock;

  cedar::aux::DoubleVectorParameterPtr _mInitialPosition;
  cedar::aux::DoubleVectorParameterPtr _mInitialOrientation;

}; // class cedar::aux::LocalCoordinateFrame

#endif // CEDAR_AUX_LOCAL_COORDINATE_FRAME_H
