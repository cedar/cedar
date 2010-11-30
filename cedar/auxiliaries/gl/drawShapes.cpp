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

void cedar::aux::gl::drawBlock(const double length, const double width, const double height, const bool wireFrame)
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glBegin(GL_QUADS);
	// front
	glNormal3d(-1.0, 0.0, 0.0);
	glVertex3d(-length/2, -width/2, -height/2);
	glVertex3d(-length/2, width/2, -height/2);
	glVertex3d(-length/2, width/2, height/2);
	glVertex3d(-length/2, -width/2, height/2);
	// right
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(-length/2, width/2, -height/2);
	glVertex3d(length/2, width/2, -height/2);
	glVertex3d(length/2, width/2, height/2);
	glVertex3d(-length/2, width/2, height/2);
	// bottom
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(-length/2, -width/2, -height/2);
	glVertex3d(length/2, -width/2, -height/2);
	glVertex3d(length/2, width/2, -height/2);
	glVertex3d(-length/2, width/2, -height/2);
	// back
	glNormal3d(1.0, 0.0, 0.0);
	glVertex3d(length/2, -width/2, -height/2);
	glVertex3d(length/2, width/2, -height/2);
	glVertex3d(length/2, width/2, height/2);
	glVertex3d(length/2, -width/2, height/2);
	// left
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(-length/2, -width/2, -height/2);
	glVertex3d(length/2, -width/2, -height/2);
	glVertex3d(length/2, -width/2, height/2);
	glVertex3d(-length/2, -width/2, height/2);
	// top
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(-length/2, -width/2, height/2);
	glVertex3d(length/2, -width/2, height/2);
	glVertex3d(length/2, width/2, height/2);
	glVertex3d(-length/2, width/2, height/2);
	glEnd();
  
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawCone(const double floor,
                              const double ceiling,
                              const double radiusFloor,
                              const double radiusCeiling,
                              const int slices,
                              const bool wireFrame)
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
  // TODO: check whether these have to be static ... what does static mean exactly anyways
	static GLUquadric* quadric = gluNewQuadric();
	glTranslatef(0.0, 0.0, floor);
	gluCylinder(quadric, radiusFloor, radiusCeiling, ceiling - floor, slices, 1);
	glTranslatef(0.0, 0.0, -floor);
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

template<typename T>
void cedar::aux::gl::drawCone(const cv::Mat start,
                              const cv::Mat end,
                              const double radiusStart,
                              const double radiusEnd,
                              const int slices,
                              const bool wireFrame)
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
template void cedar::aux::gl::drawCone<double>(const cv::Mat,
                                               const cv::Mat,
                                               const double,
                                               const double,
                                               const int,
                                               const bool);
template void cedar::aux::gl::drawCone<float>(const cv::Mat,
                                              const cv::Mat,
                                              const double,
                                              const double,
                                              const int,
                                              const bool);

void cedar::aux::gl::drawSphere(const  double radius,
                                const int slices,
                                const int stacks,
                                const bool wireFrame)
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	static GLUquadric* quadric = gluNewQuadric();
	gluSphere(quadric, radius, slices, stacks);
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawDisk(const double innerRadius, 
                              const double outerRadius,
                              const int slices,
                              const int loops,
                              const bool invert,
                              const bool wireFrame)
{
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (invert)
	{
		glRotated(180, 1, 0, 0);
	}
	static GLUquadric* quadric = gluNewQuadric();
	gluDisk(quadric, innerRadius, outerRadius, slices, loops);
	if (invert)
	{
		glRotated(180, 1, 0, 0);
	}
	if (wireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void cedar::aux::gl::drawPyramid(const double length,
                                 const double width,
                                 const double height,
                                 const bool wireFrame)
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

void cedar::aux::gl::drawPrism(const double width, const double height, const bool wireFrame)
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

void cedar::aux::gl::drawTorus(const double radius,
                               const double thickness,
                               const int slices,
                               const int stacks,
                               const bool wireFrame)
{
  // TODO: for small sizes, the reflections are not right
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
      glNormal3d(-thickness * sin((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ),
                 -thickness * cos((i + 0.5) * alpha) * thickness * cos((j + 0.5) * beta ),
                 thickness * sin((j + 0.5) * beta )
                );
      
      glVertex3d((radius - thickness * cos(j*beta)) * sin(i*alpha),
                 (radius - thickness * cos(j*beta)) * cos(i*alpha),
                 thickness * sin(j*beta)
                );
      glVertex3d((radius - thickness * cos(j*beta)) * sin((i+1)*alpha),
                 (radius - thickness * cos(j*beta)) * cos((i+1)*alpha),
                 thickness * sin(j*beta) 
                );
      glVertex3d((radius - thickness * cos((j+1)*beta)) * sin((i+1)*alpha),
                 (radius - thickness * cos((j+1)*beta)) * cos((i+1)*alpha), 
                 thickness * sin((j+1)*beta)
                );
      glVertex3d((radius - thickness * cos((j+1)*beta)) * sin(i*alpha),
                 (radius - thickness * cos((j+1)*beta)) * cos(i*alpha),
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

void cedar::aux::gl::drawEllipse(const double a,
                                 const double b,
                                 const double thickness,
                                 const int slices,
                                 const int stacks,
                                 const bool wireFrame)
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

