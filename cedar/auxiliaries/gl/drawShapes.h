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

    File:        drawShapes.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 19

    Description: Functions drawing simple geometric shapes in the current OpenGL coordinate frame

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_DRAW_SHAPES_H
#define CEDAR_AUX_GL_DRAW_SHAPES_H

#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

namespace cedar
{
  namespace aux
  {
    namespace gl
    {
      /*!@brief changes the current color to the specified RGB values
       * @param R R value of RGB color
       * @param G G value of RGB color
       * @param B B value of RGB color
       */
      CEDAR_AUX_LIB_EXPORT void setColor(float R, float G, float B);

      /*!@brief  draws a rectangular block in the current coordinate frame
       * @param length extension in x-direction of current coordinate frame
       * @param width extension in y-direction of current coordinate frame
       * @param height extension in z-direction of current coordinate frame
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      CEDAR_AUX_LIB_EXPORT void drawBlock(float length, float width, float height, bool wireFrame = false);
      
      /*!@brief  draws a rectangular block in the current coordinate frame
       * @param front extension in positive x-direction of current coordinate frame
       * @param back extension in negative x-direction of current coordinate frame
       * @param right extension in positive y-direction of current coordinate frame
       * @param left extension in negative y-direction of current coordinate frame
       * @param up extension in positive z-direction of current coordinate frame
       * @param down extension in negative z-direction of current coordinate frame
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      CEDAR_AUX_LIB_EXPORT void drawBlock
      (
        float front,
        float back,
        float right,
        float left,
        float up,
        float down,
        bool wireFrame = false
      );

      /*!@brief  draws a cone in direction of the z-axis of the current coordinate frame
       * @param floor distance from the floor to the xy-plane along the z-axis
       * @param ceiling distance from the xy-plane to the ceiling along the z-axis
       * @param radiusFloor radius of the cone at the lower end
       * @param radiusCeiling radius of the cone at the upper end
       * @param slices number of vertical cuts used to approximate the cone walls
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      CEDAR_AUX_LIB_EXPORT
      void drawCone(
                     float floor,
                     float ceiling,
                     float radiusFloor,
                     float radiusCeiling,
                     int slices,
                     bool wireFrame = false
                   );
      
      /*!@brief  draws a cone around an arbitrary line segment in the current coordinate frame
       * @param start start of the line segment
       * @param end end of the line segment
       * @param radiusStart radius of the cone at the start point
       * @param radiusEnd radius of the cone at the end point
       * @param slices number of cuts parallel to the line used to approximate the cone walls
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      template<typename T>
      void drawCone(
                     const cv::Mat& start,
                     const cv::Mat& end,
                     float radiusStart,
                     float radiusEnd,
                     int slices,
                     bool wireFrame = false
                   );

      /*!@brief draws an arrow between two arbitrary points in the current coordinate frame
       * @param start start of the arrow
       * @param end end of the arrow
       * @param shaftRadius radius of the arrow shaft
       * @param headRadius radius of the arrow head
       * @param headLength length of the arrow head
       * @param patches number of cuts parallel to the line used to approximate the cone walls
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      template<typename T>
      void drawArrow(
                      const cv::Mat& start,
                      const cv::Mat& end,
                      float shaftRadius,
                      float headRadius,
                      float headLength,
                      int patches,
                      bool wireFrame = false
                    );
      
      /*!@brief  draws a sphere around the origin of the current coordinate frame
       * @param radius radius of the sphere
       * @param slices number of longitudinal cuts used to approximate the sphere walls
       * @param stacks number of latitudinal cuts used to approximate the sphere walls
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      CEDAR_AUX_LIB_EXPORT
      void drawSphere(
                       float radius,
                       int slices,
                       int stacks,
                       bool wireFrame = false
                     );
      
      /*!@brief  draws a holed disk (i.e. ring) in the xy-plane around the origin of the current coordinate frame
       * @param innerRadius radius of the inner empty disk
       * @param outerRadius radius of the whole disk
       * @param slices number of radial cuts used to approximate the disk
       * @param loops number of concentric circles used to approximate the disk
       * @param invert turns the disk so it is visible from below instead of from above
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */
      CEDAR_AUX_LIB_EXPORT
      void drawDisk(
                     float innerRadius,
                     float outerRadius,
                     int slices,
                     int loops,
                     bool invert = false,
                     bool wireFrame = false
                   );
      
      /*!@brief  draws a pyramid with rectangular base, base is centered at origin of object frame
       * @param length base extension in x-direction of current coordinate frame
       * @param width base extension in y-direction of current coordinate frame
       * @param height extension in z-direction of current coordinate frame, from origin to tip of the pyramid
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */ 
      CEDAR_AUX_LIB_EXPORT
      void drawPyramid(
                        float length,
                        float width,
                        float height,
                        bool wireFrame = false
                      );
      
      /*!@brief  draws a prism with an equilateral triangle as base, in xy-plane, prism extends in positive z-direction
       * @param width edge length of the equilateral triangle forming the prism's base
       * @param height extension of the prism in z-direction of the current frame
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */ 
      CEDAR_AUX_LIB_EXPORT
      void drawPrism(float width, float height, bool wireFrame = false);
      
      /*!@brief  draws a torus around a circle in the xy-plane of the current frame
       * @param radius the radius of the circle in the xy-plane the torus is drawn around
       * @param thickness distance of the torus walls to the circle in the xy-plane
       * @param slices number of orthogonal cuts through the torus (i.e. hyperplanes orthogonal to the xy-plane)
       * @param stacks number of trapezoid plates used to approximate cylinder cut between two slices
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */ 
      CEDAR_AUX_LIB_EXPORT
      void drawTorus(
                      float radius,
                      float thickness,
                      int slices,
                      int stacks,
                      bool wireFrame = false
                    );
      
      /*!@brief  draws a torus around an ellipse in the xy-plane of the current frame
       * @param a extension of the ellipse in x-direction of the current frame
       * @param b extension of the ellipse in y-direction of the current frame
       * @param thickness distance of the ellipse walls to the ellipse in the xy-plane
       * @param slices number of orthogonal cuts through the torus (i.e. hyperplanes orthogonal to the xy-plane)
       * @param stacks number of trapezoid plates used to approximate cylinder cut between two slices
       * @param wireFrame decides whether to draw full surfaces or only a wire frame
       */ 
      CEDAR_AUX_LIB_EXPORT
      void drawEllipse(
                        float a,
                        float b,
                        float thickness,
                        int slices,
                        int stacks,
                        bool wireFrame = false
                      );

      /*!@brief draws axes of the current coordinate frame
       * @param length length of each arrow in direction of the base vectors
       */
      CEDAR_AUX_LIB_EXPORT
      void drawAxes(float length);

      /*!@brief draws a hair line cross along the axes of the current coordinate frame
       * @param length length of each line
       * @param width width of each line
       */
      CEDAR_AUX_LIB_EXPORT
      void drawCross(float length, float width = 1);
    }
  }
}
#endif // CEDAR_AUX_GL_DRAW_SHAPES_H
