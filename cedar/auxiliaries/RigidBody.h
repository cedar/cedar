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

    File:        RigidBody.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: header of cedar::aux::RigidBody class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_RIGID_BODY_H
#define CEDAR_AUX_RIGID_BODY_H

// CEDAR INCLUDES
#include "namespace.h"
#include "Base.h"
#include "ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>

/*!@brief Provides the geometry of a rigid object
 *
 * rigid between world coordinate frame and object coordinate frame
 */
class cedar::aux::RigidBody : public QObject, public cedar::aux::ConfigurationInterface
{
private:
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  RigidBody();
  //!@brief constructor for setting saved to a configuration file
  RigidBody(const std::string& configFileName);

  //!@brief Destructor
  virtual ~RigidBody();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief returns position of the object in homogeneous coordinates
  cv::Mat getPosition();

  //!@brief returns x-position of the object frame origin in world frame
  double getPositionX() const;

  //!@brief returns y-position of the object frame origin in world frame
  double getPositionY() const;

  //!@brief returns z-position of the object frame origin in world frame
  double getPositionZ() const;

  //!@brief returns object frame orientation as a unit quaternion
  double getOrientationQuaternion(unsigned int component);

  //!@brief returns object frame orientation as a unit quaternion
  cv::Mat getOrientationQuaternion();

  //!@brief returns the \f$4 \times 4\f$ rigid transformation matrix of the object frame relative to the world frame
  cv::Mat getTransformation();

public slots:
  /*!@brief set the position of the object frame origin in the world frame
   * @param x    coordinates of
   * @param y    value for green channel in RGB color
   * @param z    value for blue channel in RGB color
   */
  void setPosition(const double x, const double y, const double z);

  /*!@brief set the position of the object frame origin in the world frame
   * @param position    new position in homogeneous coordinates
   */
  void setPosition(const cv::Mat& position);

  /*!@brief set the orientation of the object frame, given as a unit quaternion
   * @param quaternion    new unit quaternion for the rotation, 4x1 matrix with norm 1
   */
  void setOrientationQuaternion(const cv::Mat quaternion);

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
  //!@brief recalculates the rigid transformation to the object frame from position and orientation
  void updateTransformation();
  //!@brief initialization function
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief rigid transformation to the object
  cv::Mat mTransformation;

private:
  cv::Mat mPosition; // position of the point obstacle, in homogeneous coordinates
  cv::Mat mOrientationQuaternion; // quaternion representing the orientation
  cv::Mat mTransformationTranspose; // transpose equivalent to representation compatible with OpenGl

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! lock for thread safety
  QReadWriteLock mLock;
  //! position
  std::vector<double> _mPosition;
  //! orientation
  std::vector<double> _mOrientation;

}; // class cedar::aux::RigidBody

#endif // CEDAR_AUX_OBJECT_H
