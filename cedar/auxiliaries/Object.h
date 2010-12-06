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

    File:        Object.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: header of cedar::aux::Object class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_H
#define CEDAR_AUX_OBJECT_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Base.h"
#include "ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QObject>
#include <cv.h>


/*!@brief Provides the geometry of a rigid object
 *
 * rigid between world coordinate frame and object coordinate frame
 */
class cedar::aux::Object : public QObject, public cedar::aux::ConfigurationInterface
{
private:

  Q_OBJECT;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Object();
  //!@brief constructor for setting saved to a configuration file
  Object(const std::string& configFileName);

  //!@brief Destructor
  virtual ~Object();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief returns position of the object in homogeneous coordinates
  cv::Mat getPosition();

  //!@brief returns x-position of the object frame origin in world frame
  double getPositionX();

  //!@brief returns y-position of the object frame origin in world frame
  double getPositionY();

  //!@brief returns z-position of the object frame origin in world frame
  double getPositionZ();

  //!@brief returns object frame orientation as a unit quaternion
  double getOrientationQuaternion(unsigned int component);

  //!@brief returns object frame orientation as a unit quaternion
  cv::Mat getOrientationQuaternion();

  //!@brief returns the 4 \time 4 rigid transformation matrix of the object frame relative to the world frame
  cv::Mat getTransformation();

public slots:
  /*!@brief set the position of the object frame origin in the world frame
   * @param x    coordinates of
   * @param y    value for green channel in RGB color
   * @param z    value for blue channel in RGB color
   */
  void setPosition(const double x, const double y, const double z);

  //!@brief set the position of the object frame origin in the world frame
  void setPosition(const cv::Mat& position);

  //!@brief set the orientation of the object frame, given as a unit quaternion
  void setOrientationQuaternion(unsigned int component, double value);

  //!@brief set the orientation of the object frame, given as a unit quaternion
  void setOrientationQuaternion(const cv::Mat quaternion);

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
public:
  // none yet (hopefully never!)

protected:
  cv::Mat mTransformation; // rigid transformation to the object

private:
  cv::Mat mPosition; // position of the point obstacle, in homogeneous coordinates
  cv::Mat mOrientationQuaternion; // quaternion representing the orientation
  cv::Mat mTransformationTranspose; // transpose equivalent to representation compatible with OpenGl

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  //! position
  std::vector<double> _mPosition;
  //! orientation
  std::vector<double> _mOrientation;

}; // class cedar::aux::Object

#endif // CEDAR_AUX_OBJECT_H

