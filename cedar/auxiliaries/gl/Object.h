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
    Date:        2010 10 27

    Description: Virtual class for a simple object geometry for visualization with OpenGL

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_OBJECT_H
#define CEDAR_AUX_GL_OBJECT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/namespace.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Object.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QGLViewer/qglviewer.h>


/*!@brief Base class for simple OpenGL visualizations of geometric objects
 *
 */
class cedar::aux::gl::Object : public QObject
{
private:
  
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. */
  Object(cedar::aux::ObjectPtr pObject);
  
  /*!@brief destructor. */
  virtual ~Object();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  virtual void draw() = 0;
  
  //!@brief returns name of the object
  std::string getObjectName();

  //!@brief returns type of the object
  std::string getObjectType();
  
  //!@brief returns current resolution
  int resolution();
  
  //!@brief returns R value of main object color in RGB
  double colorR();
  
  //!@brief returns G value of main object color in RGB
  double colorG();
  
  //!@brief returns B value of main object color in RGB
  double colorB();
  
  //!@brief switch betwen drawing the object with full surfaces or as wire frame only
  void drawAsWireFrame(const bool state);

  //!@brief get state of object being drawn with full surfaces or as wire frame only
  bool isDrawnAsWireFrame();
  
  /*!@brief set the general resolution of the object, 10 is a usual value
   * @param value    new resolution value
   */
  void setResolution(const int value);
  
  /*!@brief sets the main color of the object, in RGB
   * @param R    value for red channel in RGB color
   * @param G    value for green channel in RGB color
   * @param B    value for blue channel in RGB color
   */
  void setColor(const double R, const double G, const double B);

  /*!@brief returns a smart pointer to the object being visualized
   *
   * @return    smart pointer to the object
   */
  cedar::aux::ObjectPtr getObject();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialization function
  virtual void init();
  
public slots:
  //!@brief turns the visibility of the object on and off
  void setVisibility(const bool state = true);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief geometric type of the object
  std::string mObjectType;

  //!@brief the object will only be drawn if this is true
  bool mIsVisible;
  //!@brief determines if the object is drawn with full surfaces or as wire frame only
  bool mIsDrawnAsWireFrame;
  //!@brief determines how well curves and surfaces are approximated (default 10)
  int mResolution;
  
  //!@brief object color in RGB, R channel
  double mColorR;
  //!@brief object color in RGB, G channel
  double mColorG;
  //!@brief object color in RGB, B channel
  double mColorB;
  
  //!@brief pointer to the geometric object that is visualized
  cedar::aux::ObjectPtr mpObject;

  //!@brief dummy matrix to hold the transpose of the current object transformation (it's what OpenGL needs)
  cv::Mat mTransformationTranspose;
};

#endif  // CEDAR_AUX_GL_OBJECT_H
