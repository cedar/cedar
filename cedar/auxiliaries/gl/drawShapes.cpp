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
 
 ----- File:        drawShapes.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 19
 
 ----- Description: functions drawing simple geometric shapes in the current OpenGL coordinate frame
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <math.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

using namespace std;
using namespace cv;

void cedar::aux::gl::setColor(double R, double G, double B)
{
  glColor4d(R, G, B, 0);
}

void cedar::aux::gl::drawBlock(double l, double w, double h, bool wireFrame)
{
	glBegin(GL_QUADS);
	// front
	glNormal3d(-1.0, 0.0, 0.0);
	glVertex3d(-l/2, -w/2, -h/2);
	glVertex3d(-l/2, w/2, -h/2);
	glVertex3d(-l/2, w/2, h/2);
	glVertex3d(-l/2, -w/2, h/2);
	// right
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(-l/2, w/2, -h/2);
	glVertex3d(l/2, w/2, -h/2);
	glVertex3d(l/2, w/2, h/2);
	glVertex3d(-l/2, w/2, h/2);
	// bottom
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(-l/2, -w/2, -h/2);
	glVertex3d(l/2, -w/2, -h/2);
	glVertex3d(l/2, w/2, -h/2);
	glVertex3d(-l/2, w/2, -h/2);
	// back
	glNormal3d(1.0, 0.0, 0.0);
	glVertex3d(l/2, -w/2, -h/2);
	glVertex3d(l/2, w/2, -h/2);
	glVertex3d(l/2, w/2, h/2);
	glVertex3d(l/2, -w/2, h/2);
	// left
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(-l/2, -w/2, -h/2);
	glVertex3d(l/2, -w/2, -h/2);
	glVertex3d(l/2, -w/2, h/2);
	glVertex3d(-l/2, -w/2, h/2);
	// top
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(-l/2, -w/2, h/2);
	glVertex3d(l/2, -w/2, h/2);
	glVertex3d(l/2, w/2, h/2);
	glVertex3d(-l/2, w/2, h/2);
	glEnd();
  


	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawCone(
                               double floor,
                               double ceiling,
                               double radiusFloor,
                               double radiusCeiling,
                               int slices,
                               bool wireFrame
                             )
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
  glTranslatef(0.0, 0.0, floor);
  GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, radiusFloor, radiusCeiling, ceiling - floor, slices, 1);
  gluDeleteQuadric(quadric);
	glTranslatef(0.0, 0.0, -floor);
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

template<typename T>
void cedar::aux::gl::drawCone(
                               const cv::Mat start,
                               const cv::Mat end,
                               double radiusStart,
                               double radiusEnd,
                               int slices,
                               bool wireFrame
                             )
{
  Mat line = (end-start)(Rect(0, 0, 1, 3)).clone();
  // if start = end do nothing
  if (norm(line) == 0)
  {
    return;
  }
  // save the current transformation
  glPushMatrix();
  // move to start of line
  glTranslated(start.at<T>(0, 0), start.at<T>(1, 0), start.at<T>(2, 0));
  // make z-axis collinear with the cone center line
  Mat z = Mat::zeros(3, 1, start.type());
  z.at<T>(2, 0) = 1;
  double alpha = acos(line.dot(z) / norm(line));
  if (alpha != 0)
  {
    Mat axis = z.cross(line);
    glRotated(alpha*180/M_PI, axis.at<T>(0, 0), axis.at<T>(1, 0), axis.at<T>(2, 0));
  }
  // draw the cone
  drawCone(0, norm(line), radiusStart, radiusEnd, slices, wireFrame);
  // return to saved transformation
  glPopMatrix();
}
template void cedar::aux::gl::drawCone<double>(
                                                const cv::Mat,
                                                const cv::Mat,
                                                double,
                                                double,
                                                int,
                                                bool
                                              );

template void cedar::aux::gl::drawCone<float>(
                                               const cv::Mat,
                                               const cv::Mat,
                                               double,
                                               double,
                                               int,
                                               bool
                                             );

template<typename T>
void cedar::aux::gl::drawArrow(
                                const cv::Mat start,
                                const cv::Mat end,
                                double shaftRadius,
                                double headRadius,
                                double headLength,
                                int patches,
                                bool wireFrame=false
                              )
{
  drawCone<T>(
               start,
               start+(1-headLength/norm(end-start))*(end-start),
               shaftRadius,
               shaftRadius,
               patches,
               wireFrame
             );
  drawCone<T>(
               start+(1-headLength/norm(end-start))*(end-start),
               end,
               headRadius,
               0,
               patches,
               wireFrame
             );

}
template void cedar::aux::gl::drawArrow<float>(
                                                const cv::Mat start,
                                                const cv::Mat end,
                                                double shaftRadius,
                                                double headRadius,
                                                double headLength,
                                                int patches,
                                                bool wireFrame=false
                                              );
template void cedar::aux::gl::drawArrow<double>(
                                                 const cv::Mat start,
                                                 const cv::Mat end,
                                                 double shaftRadius,
                                                 double headRadius,
                                                 double headLength,
                                                 int patches,
                                                 bool wireFrame=false
                                               );

void cedar::aux::gl::drawSphere(
                                 const  double radius,
                                 int slices,
                                 int stacks,
                                 bool wireFrame
                               )
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
  GLUquadric* quadric = gluNewQuadric();
	gluSphere(quadric, radius, slices, stacks);
  gluDeleteQuadric(quadric);
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawDisk(
                               double innerRadius,
                               double outerRadius,
                               int slices,
                               int loops,
                               bool invert,
                               bool wireFrame
                             )
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (invert)
	{
		glRotated(180, 1, 0, 0);
	}
  GLUquadric* quadric = gluNewQuadric();
	gluDisk(quadric, innerRadius, outerRadius, slices, loops);
  gluDeleteQuadric(quadric);
	if (invert)
	{
		glRotated(180, 1, 0, 0);
	}
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawPyramid(
                                  double length,
                                  double width,
                                  double height,
                                  bool wireFrame
                                )
{
  // TODO: normals are not calculated correctly
  // TODO: origin should be at the center of the base rectangle
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glBegin(GL_TRIANGLES);
  // front
  glNormal3d(0.0, 1.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Front)
  glVertex3d(-length/2, width/2, -height/2);      // Left Of Triangle (Front)
  glVertex3d(length/2, width/2, -height/2);       // Right Of Triangle (Front)
  
  // right side
  glNormal3d(1.0, 0.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Right)
  glVertex3d(length/2,  width/2, -height/2);			// Left Of Triangle (Right)
  glVertex3d(length/2, -width/2, -height/2);			// Right Of Triangle (Right)
  
  // back
  glNormal3d(0.0, -1.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Back)
  glVertex3d(length/2, -width/2, -height/2);			// Left Of Triangle (Back)
  glVertex3d(-length/2, -width/2, -height/2);			// Right Of Triangle (Back)
  
  // left side
  glNormal3d(-1.0, 0.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Left)
  glVertex3d(-length/2, -width/2, -height/2);			// Left Of Triangle (Left)
  glVertex3d(-length/2,  width/2, -height/2);			// Right Of Triangle (Left)
	glEnd();
  
	glBegin(GL_QUADS);
  // base
  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(-length/2,  width/2, -height/2);			// Left Of Triangle (Front)
  glVertex3d(length/2,  width/2, -height/2);			// Right Of Triangle (Front)
  glVertex3d(length/2, -width/2, -height/2);			// Right Of Triangle (Back)
  glVertex3d(-length/2, -width/2, -height/2);			// Left Of Triangle (Back)
	glEnd();
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawPrism(double width, double height, bool wireFrame)
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glBegin(GL_TRIANGLES);					// Start Drawing The Pyramid
  // bottom
  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(0, -width/2, 0);
  glVertex3d(sqrt(3)*width/2, 0, 0);
  glVertex3d(0, width/2, 0);
  // top
  glNormal3d(0.0, 0.0, 1.0);
  glVertex3d(0, -width/2, height);
  glVertex3d(sqrt(3)*width/2, 0, height);
  glVertex3d(0, width/2, height);
	glEnd();
  
	glBegin(GL_QUADS);
  // front left
  glNormal3d(.33, -.66, 0.0); // TODO: check normal
  glVertex3d(0, -width/2, height);
  glVertex3d(sqrt(3)*width/2, 0, height);
  glVertex3d(sqrt(3)*width/2, 0, 0);
  glVertex3d(0, -width/2, 0);
  // front right
  glNormal3d(.33, .66, 0.0); // TODO: check normal
  glVertex3d(sqrt(3)*width/2, 0, 0);
  glVertex3d(0, width/2, 0);
  glVertex3d(0, width/2, height);
  glVertex3d(sqrt(3)*width/2, 0, height);
  // back
  glNormal3d(-1.0, 0.0, 0.0); // TODO: correct normal
  glVertex3d(0, -width/2, 0);
  glVertex3d(0, width/2, 0);
  glVertex3d(0, width/2, height);
  glVertex3d(0, -width/2, height);
  
	glEnd();
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawTorus(
                                double radius,
                                double thickness,
                                int slices,
                                int stacks,
                                bool wireFrame
                              )
{
  // approximates the Torus with 16 3rd order Bezier surfaces, which is not perfect, but almost
  double r = radius;
  double t = thickness;
  double d = 4.0/3.0*(sqrt(2)-1);
  // first quarter
  GLfloat I_1[4][4][3] =
  {
    {{r, 0.0, t}, {r+d*t, 0.0, t},    {r+t, 0.0, d*t},    {r+t, 0.0, 0.0}},
    {{r, d*r, t}, {r+d*t, d*(r+d*t), t}, {r+t, d*(r+t), d*t}, {r+t, d*(r+t), 0.0}},
    {{d*r, r, t}, {d*(r+d*t), r+d*t, t}, {d*(r+t), r+t, d*t}, {d*(r+t), r+t, 0.0}},
    {{0.0, r, t}, {0.0, r+d*t, t},    {0.0, r+t, d*t},    {0.0, r+t, 0.0}}
  };
  GLfloat I_2[4][4][3] =
  {
    {{0.0, r, t}, {0.0, r-d*t, t},    {0.0, r-t, d*t},    {0.0, r-t, 0.0}},
    {{d*r, r, t}, {d*(r-d*t), r-d*t, t}, {d*(r-t), r-t, d*t}, {d*(r-t), r-t, 0.0}},
    {{r, d*r, t}, {r-d*t, d*(r-d*t), t}, {r-t, d*(r-t), d*t}, {r-t, d*(r-t), 0.0}},
    {{r, 0.0, t}, {r-d*t, 0.0, t},    {r-t, 0.0, d*t},    {r-t, 0.0, 0.0}}
  };
  GLfloat I_3[4][4][3] =
  {
    {{r, 0.0, -t}, {r-d*t, 0.0, -t},    {r-t, 0.0, -d*t},    {r-t, 0.0, 0.0}},
    {{r, d*r, -t}, {r-d*t, d*(r-d*t), -t}, {r-t, d*(r-t), -d*t}, {r-t, d*(r-t), 0.0}},
    {{d*r, r, -t}, {d*(r-d*t), r-d*t, -t}, {d*(r-t), r-t, -d*t}, {d*(r-t), r-t, 0.0}},
    {{0.0, r, -t}, {0.0, r-d*t, -t},    {0.0, r-t, -d*t},    {0.0, r-t, 0.0}}
  };
  GLfloat I_4[4][4][3] =
  {
    {{0.0, r, -t}, {0.0, r+d*t, -t},    {0.0, r+t, -d*t},    {0.0, r+t, -0.0}},
    {{d*r, r, -t}, {d*(r+d*t), r+d*t, -t}, {d*(r+t), r+t, -d*t}, {d*(r+t), r+t, -0.0}},
    {{r, d*r, -t}, {r+d*t, d*(r+d*t), -t}, {r+t, d*(r+t), -d*t}, {r+t, d*(r+t), -0.0}},
    {{r, 0.0, -t}, {r+d*t, 0.0, -t},    {r+t, 0.0, -d*t},    {r+t, 0.0, -0.0}}
  };

  // second quarter
  GLfloat II_1[4][4][3] =
  {
    {{0.0, r, t}, {0.0, r+d*t, t},    {0.0, r+t, d*t},    {0.0, r+t, 0.0}},
    {{-d*r, r, t}, {-d*(r+d*t), r+d*t, t}, {-d*(r+t), r+t, d*t}, {-d*(r+t), r+t, 0.0}},
    {{-r, d*r, t}, {-r-d*t, d*(r+d*t), t}, {-r-t, d*(r+t), d*t}, {-r-t, d*(r+t), 0.0}},
    {{-r, 0.0, t}, {-r-d*t, 0.0, t},    {-r-t, 0.0, d*t},    {-r-t, 0.0, 0.0}}
  };
  GLfloat II_2[4][4][3] =
  {
    {{-r, 0.0, t}, {-r+d*t, 0.0, t},    {-r+t, 0.0, d*t},    {-r+t, 0.0, 0.0}},
    {{-r, d*r, t}, {-r+d*t, d*(r-d*t), t}, {-r+t, d*(r-t), d*t}, {-r+t, d*(r-t), 0.0}},
    {{-d*r, r, t}, {-d*(r-d*t), r-d*t, t}, {-d*(r-t), r-t, d*t}, {-d*(r-t), r-t, 0.0}},
    {{0.0, r, t}, {0.0, r-d*t, t},    {0.0, r-t, d*t},    {0.0, r-t, 0.0}}
  };
  GLfloat II_3[4][4][3] =
  {
    {{0.0, r, -t}, {0.0, r-d*t, -t},    {0.0, r-t, -d*t},    {0.0, r-t, 0.0}},
    {{-d*r, r, -t}, {-d*(r-d*t), r-d*t, -t}, {-d*(r-t), r-t, -d*t}, {-d*(r-t), r-t, 0.0}},
    {{-r, d*r, -t}, {-r+d*t, d*(r-d*t), -t}, {-r+t, d*(r-t), -d*t}, {-r+t, d*(r-t), 0.0}},
    {{-r, 0.0, -t}, {-r+d*t, 0.0, -t},    {-r+t, 0.0, -d*t},    {-r+t, 0.0, 0.0}}
  };
  GLfloat II_4[4][4][3] =
  {
    {{-r, 0.0, -t}, {-r-d*t, 0.0, -t},    {-r-t, 0.0, -d*t},    {-r-t, 0.0, 0.0}},
    {{-r, d*r, -t}, {-r-d*t, d*(r+d*t), -t}, {-r-t, d*(r+t), -d*t}, {-r-t, d*(r+t), 0.0}},
    {{-d*r, r, -t}, {-d*(r+d*t), r+d*t, -t}, {-d*(r+t), r+t, -d*t}, {-d*(r+t), r+t, 0.0}},
    {{0.0, r, -t}, {0.0, r+d*t, -t},    {0.0, r+t, -d*t},    {0.0, r+t, 0.0}}
  };

  // third quarter
  GLfloat III_1[4][4][3] =
  {
    {{0.0, -r, t}, {0.0, -r-d*t, t},    {0.0, -r-t, d*t},    {0.0, -r-t, 0.0}},
    {{d*r, -r, t}, {d*(r+d*t), -r-d*t, t}, {d*(r+t), -r-t, d*t}, {d*(r+t), -r-t, 0.0}},
    {{r, -d*r, t}, {r+d*t, -d*(r+d*t), t}, {r+t, -d*(r+t), d*t}, {r+t, -d*(r+t), 0.0}},
    {{r, 0.0, t}, {r+d*t, 0.0, t},    {r+t, 0.0, d*t},    {r+t, 0.0, 0.0}}
  };
  GLfloat III_2[4][4][3] =
  {
    {{r, 0.0, t}, {r-d*t, 0.0, t},    {r-t, 0.0, d*t},    {r-t, 0.0, 0.0}},
    {{r, -d*r, t}, {r-d*t, -d*(r-d*t), t}, {r-t, -d*(r-t), d*t}, {r-t, -d*(r-t), 0.0}},
    {{d*r, -r, t}, {d*(r-d*t), -r+d*t, t}, {d*(r-t), -r+t, d*t}, {d*(r-t), -r+t, 0.0}},
    {{0.0, -r, t}, {0.0, -r+d*t, t},    {0.0, -r+t, d*t},    {0.0, -r+t, 0.0}}
  };
  GLfloat III_3[4][4][3] =
  {
    {{0.0, -r, -t}, {0.0, -r+d*t, -t},    {0.0, -r+t, -d*t},    {0.0, -r+t, 0.0}},
    {{d*r, -r, -t}, {d*(r-d*t), -r+d*t, -t}, {d*(r-t), -r+t, -d*t}, {d*(r-t), -r+t, 0.0}},
    {{r, -d*r, -t}, {r-d*t, -d*(r-d*t), -t}, {r-t, -d*(r-t), -d*t}, {r-t, -d*(r-t), 0.0}},
    {{r, 0.0, -t}, {r-d*t, 0.0, -t},    {r-t, 0.0, -d*t},    {r-t, 0.0, 0.0}}
  };
  GLfloat III_4[4][4][3] =
  {
    {{r, 0.0, -t}, {r+d*t, 0.0, -t},    {r+t, 0.0, -d*t},    {r+t, 0.0, -0.0}},
    {{r, -d*r, -t}, {r+d*t, -d*(r+d*t), -t}, {r+t, -d*(r+t), -d*t}, {r+t, -d*(r+t), -0.0}},
    {{d*r, -r, -t}, {d*(r+d*t), -r-d*t, -t}, {d*(r+t), -r-t, -d*t}, {d*(r+t), -r-t, -0.0}},
    {{0.0, -r, -t}, {0.0, -r-d*t, -t},    {0.0, -r-t, -d*t},    {0.0, -r-t, -0.0}}
  };

  // fourth quarter
  GLfloat IV_1[4][4][3] =
  {
    {{-r, 0.0, t}, {-r-d*t, 0.0, t},    {-r-t, 0.0, d*t},    {-r-t, 0.0, 0.0}},
    {{-r, -d*r, t}, {-r-d*t, -d*(r+d*t), t}, {-r-t, -d*(r+t), d*t}, {-r-t, -d*(r+t), 0.0}},
    {{-d*r, -r, t}, {-d*(r+d*t), -r-d*t, t}, {-d*(r+t), -r-t, d*t}, {-d*(r+t), -r-t, 0.0}},
    {{0.0, -r, t}, {0.0, -r-d*t, t},    {0.0, -r-t, d*t},    {0.0, -r-t, 0.0}}
  };
  GLfloat IV_2[4][4][3] =
  {
    {{0.0, -r, t}, {0.0, -r+d*t, t},    {0.0, -r+t, d*t},    {0.0, -r+t, 0.0}},
    {{-d*r, -r, t}, {-d*(r-d*t), -r+d*t, t}, {-d*(r-t), -r+t, d*t}, {-d*(r-t), -r+t, 0.0}},
    {{-r, -d*r, t}, {-r+d*t, -d*(r-d*t), t}, {-r+t, -d*(r-t), d*t}, {-r+t, -d*(r-t), 0.0}},
    {{-r, 0.0, t}, {-r+d*t, 0.0, t},    {-r+t, 0.0, d*t},    {-r+t, 0.0, 0.0}}
  };
  GLfloat IV_3[4][4][3] =
  {
    {{-r, 0.0, -t}, {-r+d*t, 0.0, -t},    {-r+t, 0.0, -d*t},    {-r+t, 0.0, 0.0}},
    {{-r, -d*r, -t}, {-r+d*t, -d*(r-d*t), -t}, {-r+t, -d*(r-t), -d*t}, {-r+t, -d*(r-t), 0.0}},
    {{-d*r, -r, -t}, {-d*(r-d*t), -r+d*t, -t}, {-d*(r-t), -r+t, -d*t}, {-d*(r-t), -r+t, 0.0}},
    {{0.0, -r, -t}, {0.0, -r+d*t, -t},    {0.0, -r+t, -d*t},    {0.0, -r+t, 0.0}}
  };
  GLfloat IV_4[4][4][3] =
  {
    {{0.0, -r, -t}, {0.0, -r-d*t, -t},    {0.0, -r-t, -d*t},    {0.0, -r-t, 0.0}},
    {{-d*r, -r, -t}, {-d*(r+d*t), -r-d*t, -t}, {-d*(r+t), -r-t, -d*t}, {-d*(r+t), -r-t, 0.0}},
    {{-r, -d*r, -t}, {-r-d*t, -d*(r+d*t), -t}, {-r-t, -d*(r+t), -d*t}, {-r-t, -d*(r+t), 0.0}},
    {{-r, 0.0, -t}, {-r-d*t, 0.0, -t},    {-r-t, 0.0, -d*t},    {-r-t, 0.0, 0.0}}
  };



  //TODO: check which of these should move to the init function
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_AUTO_NORMAL);

  if (wireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // first quarter
  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &I_1[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &I_2[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &I_3[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &I_4[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  // second quarter
  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &II_1[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &II_2[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &II_3[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &II_4[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  // third quarter
  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &III_1[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &III_2[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &III_3[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &III_4[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  // fourth quarter
  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &IV_1[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &IV_2[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &IV_3[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &IV_4[0][0][0]);
  glMapGrid2f(slices, 0.0, 1.0, stacks, 0.0, 1.0);
  glEvalMesh2(GL_FILL, 0, slices, 0, stacks);

  if (wireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }













  // TODO: for small sizes, the reflections are not right
// 	double alpha = 2 * M_PI / slices;
//	double beta = 2 * M_PI / stacks;
//
//	if (wireFrame)
//	{
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	}
//	for (int i=0; i<slices; i++)
//	{
//		for (int j=0; j<stacks; j++)
//		{
//			glBegin(GL_QUADS);
//      glNormal3d(-thickness * sin((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ),
//                 -thickness * cos((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ),
//                 thickness * sin((j + 0.5) * beta )
//                );
//
//      glVertex3d((radius - thickness * cos(j*beta)) * sin(i*alpha),
//                 (radius - thickness * cos(j*beta)) * cos(i*alpha),
//                 thickness * sin(j*beta)
//                );
//      glVertex3d((radius - thickness * cos(j*beta)) * sin((i+1)*alpha),
//                 (radius - thickness * cos(j*beta)) * cos((i+1)*alpha),
//                 thickness * sin(j*beta)
//                );
//      glVertex3d((radius - thickness * cos((j+1)*beta)) * sin((i+1)*alpha),
//                 (radius - thickness * cos((j+1)*beta)) * cos((i+1)*alpha),
//                 thickness * sin((j+1)*beta)
//                );
//      glVertex3d((radius - thickness * cos((j+1)*beta)) * sin(i*alpha),
//                 (radius - thickness * cos((j+1)*beta)) * cos(i*alpha),
//                 thickness * sin((j+1)*beta)
//                );
//			glEnd();
//		}
//	}
//	if (wireFrame)
//	{
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	}
}

void cedar::aux::gl::drawEllipse(
                                  double a,
                                  double b,
                                  double thickness,
                                  int slices,
                                  int stacks,
                                  bool wireFrame
                                )
{
	double alpha = 2 * M_PI / slices;
	double beta = 2 * M_PI / stacks;
  
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	for (int i=0; i<slices; i++)
	{
		for (int j=0; j<stacks; j++)
		{
			glBegin(GL_QUADS);
      // TODO: check normals
      glNormal3d(-thickness * sin((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ),
                 -thickness * cos((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ), 
                 thickness * sin((j + 0.5) * beta )
                ); 
      glVertex3d((b - thickness * cos(j*beta)) * sin(i*alpha),
                 (a - thickness * cos(j*beta)) * cos(i*alpha),
                 thickness * sin(j*beta)
                );
      glVertex3d((b - thickness * cos(j*beta)) * sin((i+1)*alpha),
                 (a - thickness * cos(j*beta)) * cos((i+1)*alpha),
                 thickness * sin(j*beta)
                );
      glVertex3d((b - thickness * cos((j+1)*beta)) * sin((i+1)*alpha),
                 (a - thickness * cos((j+1)*beta)) * cos((i+1)*alpha),
                 thickness * sin((j+1)*beta)
                );
      glVertex3d((b - thickness * cos((j+1)*beta)) * sin(i*alpha),
                 (a - thickness * cos((j+1)*beta)) * cos(i*alpha),
                 thickness * sin((j+1)*beta)
                );
			glEnd();
		}
	}
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

