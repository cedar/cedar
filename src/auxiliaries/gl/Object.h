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
#include "src/auxiliaries/math/tools.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>
#include <vector>
#include "cv.h"
#include <QObject>

namespace cedar
{
  namespace aux
  {
    namespace gl
    {
      class cedar::aux::gl::Object : public QObject
      {
      private:
        
        Q_OBJECT
        
      public:
        // structors
        Object();
        
        // methods
        virtual void init();
        virtual void draw()=0;
        
        // variable access
        
        //!@brief returns name of the object
        std::string objectName();
        //!@brief returns type of the object
        std::string objectType();
        //!@brief returns x-position of the object frame origin in world frame
        double positionX();
        //!@brief returns y-position of the object frame origin in world frame
        double positionY();
        //!@brief returns z-position of the object frame origin in world frame
        double positionZ();
        //!@brief returns alpha (first) angle of the object orientation in ???-Euler angles
        double orientationAngleAlpha();
        //!@brief returns beta (second) angle of the object orientation in ???-Euler angles
        double orientationAngleBeta();
        //!@brief returns gamma (third) angle of the object orientation in ???-Euler angles
        double orientationAngleGamma();
        //!@brief returns the 4 \time 4 rigid transformation matrix of the object frame relative to the world frame
        cv::Mat transformation();
        //!@brief returns R value of main object color in RGB
        double colorR();
        //!@brief returns G value of main object color in RGB
        double colorG();
        //!@brief returns B value of main object color in RGB
        double colorB();
        
        //!@brief switch betwen drawing the object with full surfaces or as wire frame only
        void drawAsWireFrame(const bool state);
        //!@brief set the general resolution of the object, 10 is a usual value
        void setResolution(const int value);
        //!@brief sets the main color of the object, in RGB
        void setColor(const double R, const double G, const double B);
        //!@brief set the position of the object frame origin in the world frame
        void setPosition(const double x, const double y, const double z);
        //!@brief set the position of the object frame origin in the world frame
        void setPosition(const cv::Mat position);
        //!@brief set the orientation of the object frame, given in ??? Euler angles
        void setOrientationAngles(const double alpha, const double beta, const double gamma);
        //!@brief set the orientation of the object frame, given in ??? Euler angles
        void setOrientationAngles(const cv::Mat angles);
       
      private:
        //!@brief recalculates the rigid transformation to the object frame from position and orientation
        void updateTransformation();
        
      public slots:
        //!@brief turns the visibility of the object on and off
        void setVisibility(const bool state = true);
        
      signals:
        //!@ emitted when something in the object changed
        void updated();
        
      public:
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


































