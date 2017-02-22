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
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LocalCoordinateFrame.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QReadWriteLock>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>

/*!@brief Provides the geometry of a rigid object
 *
 * rigid between world coordinate frame and object coordinate frame
 */
class cedar::aux::LocalCoordinateFrame : public QObject, public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
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

  /*!@brief returns translation of the object in homogeneous coordinates
   * @return translation of the local frame origin in the world frame
   */
  cedar::unit::LengthMatrix getTranslation() const;

  /*!@brief returns x-translation of the local frame origin in world frame
   * @return translation of the local frame origin in the world frame
   */
  cedar::unit::Length getTranslationX() const;

  /*!@brief returns y-translation of the local frame origin in world frame
   * @return translation of the local frame origin in the world frame in x-direction
   */
  cedar::unit::Length getTranslationY() const;

  /*!@brief returns z-translation of the local frame origin in world frame
   * @return translation of the local frame origin in the world frame in y-direction
   */
  cedar::unit::Length getTranslationZ() const;

  /*!@brief returns rotation of the local frame origin in world frame
   * @return rotation of the local frame relative to the world frame
   */
  cv::Mat getRotation() const;

  /*!@brief returns the \f$4 \times 4\f$ rigid transformation matrix of the object frame relative to the world frame
   * @return transformation of the local frame relative to the world frame
   */
  cv::Mat getTransformation() const;

  /*!@brief sets the transformation matrix
   * @param transformation \f$4 \times 4\f$ rigid transformation matrix of the object frame relative to the world frame
   */
  void setTransformation(cv::Mat transformation);

public slots:
  //!@brief updates the model
  virtual void update();

  /*!@brief set the translation of the object frame origin in the world frame
   * @param x new translation along the x-axis
   * @param y new translation along the y-axis
   * @param z new translation along the z-axis
   */
  void setTranslation
       (
         const cedar::unit::Length& x,
         const cedar::unit::Length& y,
         const cedar::unit::Length& z
       );

  /*!@brief set the translation of the object frame origin in the world frame
   * @param translation new translation vector in homogeneous coordinates
   */
  void setTranslation(const cedar::unit::LengthMatrix& translation);

  /*!@brief set the translation of the object frame origin in the world frame
   * @param translation new translation vector in homogeneous coordinates
   */
  void setTranslation(const std::vector<float> &translation);

  /*!@brief add the provided values to the translation of the local frame origin in the world frame
   * @param x additional translation along the x-axis
   * @param y additional translation along the y-axis
   * @param z additional translation along the z-axis
   */
  void translate
       (
         const cedar::unit::Length& x,
         const cedar::unit::Length& y,
         const cedar::unit::Length& z
       );

  /*!@brief set the translation of the object frame origin in the world frame
   * @param translation additional translation in homogeneous coordinates
   */
  void translate(const cedar::unit::LengthMatrix& translation);

  /*!@brief set the translation of the object frame origin in the world frame
   * @param translation additional translation in homogeneous coordinates
   */
  void translate(const std::vector<cedar::unit::Length >& translation);

  /*!@brief set the rotation of the object frame in the world frame
   * @param rotation new rotation matrix
   */
  void setRotation(const cv::Mat& rotation);

  /*!@brief set the rotation of the object frame in the world frame
   * @param  rotation new rotation matrix
   */
  void setRotation(const std::vector<float>& rotation);

  /*!@brief rotates the object around one of the main axes of the object frame
   * @param axis    index of the axis to rotate around, between 0 and 2
   * @param angle    value of angle that the object is rotated by, in radians
   */
  void rotate(unsigned int axis, const cedar::unit::PlaneAngle& angle);

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

  //! lock for thread safety
  mutable QReadWriteLock mLock;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::DoubleVectorParameterPtr _mInitialTranslation;
  cedar::aux::DoubleVectorParameterPtr _mInitialRotation;

}; // class cedar::aux::LocalCoordinateFrame

#endif // CEDAR_AUX_LOCAL_COORDINATE_FRAME_H
