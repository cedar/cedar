/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Object.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 27
 
 ----- Description: virtual class for a simple object geometry for visualization with OpenGL
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_OBJECT_H
#define CEDAR_AUX_GL_OBJECT_H

// LOCAL INCLUDES
#include "namespace.h"
#include "cedar/auxiliaries/math/tools.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <vector>
#include <cv.h>
#include <QObject>
#include <QGLViewer/qglviewer.h>

class cedar::aux::gl::Object : public QObject
{
private:
  
  Q_OBJECT
  
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. */
  Object();
  
  /*!@brief destructor. */
  virtual ~Object(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  virtual void draw()=0;
  
  //!@brief returns name of the object
  std::string objectName();

  //!@brief returns type of the object
  std::string objectType();
  
  //!@brief returns current resolution
  int resolution();
  
  //!@brief returns R value of main object color in RGB
  double colorR();
  
  //!@brief returns G value of main object color in RGB
  double colorG();
  
  //!@brief returns B value of main object color in RGB
  double colorB();
  
  //!@brief returns position of the object in homogeneous coordinates
  cv::Mat position();
  
  //!@brief returns x-position of the object frame origin in world frame
  double positionX();
  
  //!@brief returns y-position of the object frame origin in world frame
  double positionY();
  
  //!@brief returns z-position of the object frame origin in world frame
  double positionZ();
  
  //!@brief returns object frame orientation as vector of ???-Euler angles
  cv::Mat orientationAngles();
  
  //!@brief returns alpha (first) angle of the object orientation in ???-Euler angles
  double orientationAngleAlpha();
  
  //!@brief returns beta (second) angle of the object orientation in ???-Euler angles
  double orientationAngleBeta();
  
  //!@brief returns gamma (third) angle of the object orientation in ???-Euler angles
  double orientationAngleGamma();
  
  //!@brief returns the 4 \time 4 rigid transformation matrix of the object frame relative to the world frame
  cv::Mat transformation();
  
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
  
  /*!@brief set the position of the object frame origin in the world frame
   * @param x    coordinates of 
   * @param y    value for green channel in RGB color
   * @param z    value for blue channel in RGB color
   */
  void setPosition(const double x, const double y, const double z);
  
  //!@brief set the position of the object frame origin in the world frame
  void setPosition(const cv::Mat& position);
  
  //!@brief set the orientation of the object frame, given in ??? Euler angles
  void setOrientationAngles(const double alpha, const double beta, const double gamma);
  
  //!@brief set the orientation of the object frame, given in ??? Euler angles
  void setOrientationAngles(const cv::Mat angles);
 
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  virtual void init();
  //!@brief recalculates the rigid transformation to the object frame from position and orientation
  void updateTransformation();
  
public slots:
  //!@brief turns the visibility of the object on and off
  void setVisibility(const bool state = true);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  std::string mName;
  std::string mObjectType;

  bool mIsVisible;
  bool mIsDrawnAsWireFrame;
  int mResolution; // determines how nice the image is drawn
  
  double mColorR;
  double mColorG;
  double mColorB;
  
  cv::Mat mPosition; // position of the point obstacle, in homogeneous coordinates
  cv::Mat mOrientationAngles; // vector of Euler angles
  cv::Mat mTransformation; // rigid transformation to the object
  cv::Mat mTransformationTranspose; // transpose equivalent to representation compatible with OpenGl 

  
};

#endif  // CEDAR_AUX_GL_OBJECT_H


































