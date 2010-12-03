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

 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        drawShapes.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 19
 
 ----- Description: functions drawing simple geometric shapes in the current OpenGL coordinate frame
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_DRAW_SHAPES_H
#define CEDAR_AUX_GL_DRAW_SHAPES_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include "cv.h"

//TODO: add setColor function, so this is the only file that needs to include gl
namespace cedar
{
  namespace aux
  {
    namespace gl
    {
      /*!@brief changes the current color to the specified RGB values
       * @param R    R value of RGB color
       * @param G    G value of RGB color
       * @param B    B value of RGB color
       */
      void setColor(double R, double G, double B);

      /*!@brief	draws a rectangular block in the current coordinate frame
       * @param length    extension in x-direction of current coordinate frame
       * @param width    extension in y-direction of current coordinate frame
       * @param height    extension in z-direction of current coordinate frame
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */
      void drawBlock(const double length, const double width, const double height, const bool wireFrame=false);
      
      /*!@brief	draws a cone in direction of the z-axis of the current coordinate frame
       * @param floor    distance from the floor to the xy-plane along the z-axis
       * @param ceiling    distance from the xy-plane to the ceiling along the z-axis
       * @param radiusFloor    radius of the cone at the lower end
       * @param radiusCeiling    radius of the cone at the upper end
       * @param patches    number of vertical cuts used to approximate the cone walls
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */
      void drawCone(const double floor,
                    const double ceiling,
                    const double radiusFloor,
                    const double radiusCeiling,
                    const int slices,
                    const bool wireFrame=false);
      
      /*!@brief	draws a cone around an arbitrary line segment in the current coordinate frame
       * @param start    start of the line segment
       * @param end    end of the line segment
       * @param radiusStart    radius of the cone at the start point
       * @param radiusEnd    radius of the cone at the end point
       * @param patches    number of cuts parallel to the line used to approximate the cone walls
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */
      template<typename T>
      void drawCone(const cv::Mat start,
                    const cv::Mat end,
                    const double radiusStart,
                    const double radiusEnd,
                    const int slices,
                    const bool wireFrame=false);
      
      /*!@brief	draws a sphere around the origin of the current coordinate frame
       * @param radius    radius of the sphere
       * @param slices    number of longitudinal cuts used to approximate the sphere walls
       * @param stacks    number of latitudinal cuts used to approximate the sphere walls
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */
      void drawSphere(const  double radius,
                      const int slices,
                      const int stacks,
                      const bool wireFrame=false);
      
      /*!@brief	draws a holed disk (i.e. ring) in the xy-plane around the origin of the current coordinate frame
       * @param innerRadius    radius of the inner empty disk
       * @param outerRadius    radius of the whole disk
       * @param slices    number of radial cuts used to approximate the disk
       * @param loops    number of concentric circles used to approximate the disk
       * @param invert    turns the disk so it is visible from below instead of from above
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */
      void drawDisk(const double innerRadius, 
                    const double outerRadius,
                    const int slices,
                    const int loops,
                    const bool invert=false,
                    const bool wireFrame=false);
      
      /*!@brief	draws a pyramid with rectangular base, base is centered at origin of object frame
       * @param length    base extension in x-direction of current coordinate frame
       * @param width    base extension in y-direction of current coordinate frame
       * @param height    extension in z-direction of current coordinate frame, from origin to tip of the pyramid
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */ 
      void drawPyramid(const double length,
                       const double width,
                       const double height,
                       const bool wireFrame=false);
      
      /*!@brief	draws a prism with an equilateral triangle as base, in xy-plane, prism extends in positive z-direction
       * @param width    edge length of the equilateral triangle forming the prism's base
       * @param height    extension of the prism in z-direction of the current frame
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */ 
      void drawPrism(const double width, const double height, const bool wireFrame=false );
      
      /*!@brief	draws a torus around a circle in the xy-plane of the current frame
       * @param radius    the radius of the circle in the xy-plane the torus is drawn around
       * @param thickness    distance of the torus walls to the circle in the xy-plane
       * @param slices    number of orthogonal cuts through the torus (i.e. hyperplanes orthogonal to the xy-plane)
       * @param stacks    number of trapezoid plates used to approximate cylinder cut between two slices
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */ 
      void drawTorus(const double radius,
                     const double thickness,
                     const int slices,
                     const int stacks,
                     const bool wireFrame=false);
      
      /*!@brief	draws a torus around an ellipse in the xy-plane of the current frame
       * @param a    extension of the ellipse in x-direction of the current frame
       * @param b    extension of the ellipse in y-direction of the current frame
       * @param thickness    distance of the ellipse walls to the ellipse in the xy-plane
       * @param slices    number of orthogonal cuts through the torus (i.e. hyperplanes orthogonal to the xy-plane)
       * @param stacks    number of trapezoid plates used to approximate cylinder cut between two slices
       * @param wireFrame    decides whether to draw full surfaces or only a wire frame
       */ 
      void drawEllipse(const double a,
                       const double b,
                       const double thickness,
                       const int slices,
                       const int stacks,
                       const bool wireFrame=false);
    };
  };
};
#endif // CEDAR_AUX_GL_DRAW_SHAPES_H
