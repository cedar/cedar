/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        drawShapes.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 19

    Description: Functions drawing simple geometric shapes in the current OpenGL coordinate frame

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES

void cedar::aux::gl::setColor(double R, double G, double B)
{
  glColor4d(R, G, B, 0);
}

void cedar::aux::gl::drawBlock(double l, double w, double h, bool wireFrame)
{
  if (wireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
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

void cedar::aux::gl::drawBlock
(
  double front,
  double back,
  double right,
  double left,
  double up,
  double down,
  bool wireFrame
)
{
  glTranslated(front, right, up); // translate to corner
  glTranslated(-(front + back) * 0.5, -(right + left) * 0.5, -(up + down) * 0.5); // translate to corner
  cedar::aux::gl::drawBlock(front + back, right + left, up + down, wireFrame);
  glTranslated((front + back) * 0.5, (right + left) * 0.5, (up + down) * 0.5); // translate to center
  glTranslated(-front, -right, -up); // translate to previous point
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
                               const cv::Mat& start,
                               const cv::Mat& end,
                               double radiusStart,
                               double radiusEnd,
                               int slices,
                               bool wireFrame
                             )
{
  cv::Mat line = (end-start)(cv::Rect(0, 0, 1, 3));
  // if start = end do nothing
  if (cv::norm(line) == 0)
  {
    return;
  }
  // save the current transformation
  glPushMatrix();
  // move to start of line
  glTranslated(start.at<T>(0, 0), start.at<T>(1, 0), start.at<T>(2, 0));
  // make z-axis collinear with the cone center line
  cv::Mat z = cv::Mat::zeros(3, 1, start.type());
  z.at<T>(2, 0) = 1;
  double alpha = acos( line.dot(z) / cv::norm(line) );
  if (alpha != 0)
  {
    cv::Mat axis = z.cross(line);
    glRotated(alpha*180/cedar::aux::math::pi, axis.at<T>(0, 0), axis.at<T>(1, 0), axis.at<T>(2, 0));
  }
  // draw the cone
  drawCone(0, cv::norm(line), radiusStart, radiusEnd, slices, wireFrame);
  // return to saved transformation
  glPopMatrix();
}

template CEDAR_AUX_LIB_EXPORT
  void cedar::aux::gl::drawCone<double>(
                                          const cv::Mat&,
                                          const cv::Mat&,
                                          double,
                                          double,
                                          int,
                                          bool
                                        );

template CEDAR_AUX_LIB_EXPORT
  void cedar::aux::gl::drawCone<float>(
                                         const cv::Mat&,
                                         const cv::Mat&,
                                         double,
                                         double,
                                         int,
                                         bool
                                       );

template<typename T>
void cedar::aux::gl::drawArrow(
                                const cv::Mat& start,
                                const cv::Mat& end,
                                double shaftRadius,
                                double headRadius,
                                double headLength,
                                int patches,
                                bool wireFrame
                              )
{
  drawCone<T>(
               start,
               start+(1-headLength/cv::norm(end-start))*(end-start),
               shaftRadius,
               shaftRadius,
               patches,
               wireFrame
             );
  drawCone<T>(
               start+(1-headLength/cv::norm(end-start))*(end-start),
               end,
               headRadius,
               0,
               patches,
               wireFrame
             );

}
template CEDAR_AUX_LIB_EXPORT void cedar::aux::gl::drawArrow<float>(
                                                                     const cv::Mat& start,
                                                                     const cv::Mat& end,
                                                                     double shaftRadius,
                                                                     double headRadius,
                                                                     double headLength,
                                                                     int patches,
                                                                     bool wireFrame
                                                                   );
template CEDAR_AUX_LIB_EXPORT void cedar::aux::gl::drawArrow<double>(
                                                                      const cv::Mat& start,
                                                                      const cv::Mat& end,
                                                                      double shaftRadius,
                                                                      double headRadius,
                                                                      double headLength,
                                                                      int patches,
                                                                      bool wireFrame
                                                                    );

void cedar::aux::gl::drawSphere(
                                 double radius,
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

//!@todo origin should be at the center of the base rectangle
void cedar::aux::gl::drawPyramid(
                                  double length,
                                  double width,
                                  double height,
                                  bool wireFrame
                                )
{
  if (wireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glBegin( GL_TRIANGLES);
  // front
  glNormal3d(0.0, 1.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Front)
  glVertex3d(-length/2, width/2, -height/2);      // Left Of Triangle (Front)
  glVertex3d(length/2, width/2, -height/2);       // Right Of Triangle (Front)
  
  // right side
  glNormal3d(1.0, 0.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Right)
  glVertex3d(length/2,  width/2, -height/2);      // Left Of Triangle (Right)
  glVertex3d(length/2, -width/2, -height/2);      // Right Of Triangle (Right)
  
  // back
  glNormal3d(0.0, -1.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Back)
  glVertex3d(length/2, -width/2, -height/2);      // Left Of Triangle (Back)
  glVertex3d(-length/2, -width/2, -height/2);      // Right Of Triangle (Back)
  
  // left side
  glNormal3d(-1.0, 0.0, 0.0);
  glVertex3d(0, 0, height/2);                     // Top Of Triangle (Left)
  glVertex3d(-length/2, -width/2, -height/2);      // Left Of Triangle (Left)
  glVertex3d(-length/2,  width/2, -height/2);      // Right Of Triangle (Left)
  glEnd();
  
  glBegin(GL_QUADS);
  // base
  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(-length/2,  width/2, -height/2);      // Left Of Triangle (Front)
  glVertex3d(length/2,  width/2, -height/2);      // Right Of Triangle (Front)
  glVertex3d(length/2, -width/2, -height/2);      // Right Of Triangle (Back)
  glVertex3d(-length/2, -width/2, -height/2);      // Left Of Triangle (Back)
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
  glBegin(GL_TRIANGLES);          // Start Drawing The Pyramid
  // bottom
  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(0, -width/2, 0);
  glVertex3d(sqrt(3.0)*width/2, 0, 0);
  glVertex3d(0, width/2, 0);
  // top
  glNormal3d(0.0, 0.0, 1.0);
  glVertex3d(0, -width/2, height);
  glVertex3d(sqrt(3.0)*width/2, 0, height);
  glVertex3d(0, width/2, height);
  glEnd();
  
  glBegin(GL_QUADS);
  // front left
  glNormal3d(.33, -.66, 0.0);
  glVertex3d(0, -width/2, height);
  glVertex3d(sqrt(3.0)*width/2, 0, height);
  glVertex3d(sqrt(3.0)*width/2, 0, 0);
  glVertex3d(0, -width/2, 0);
  // front right
  glNormal3d(.33, .66, 0.0);
  glVertex3d(sqrt(3.0)*width/2, 0, 0);
  glVertex3d(0, width/2, 0);
  glVertex3d(0, width/2, height);
  glVertex3d(sqrt(3.0)*width/2, 0, height);
  // back
  glNormal3d(-1.0, 0.0, 0.0);
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
  cedar::aux::gl::drawEllipse(radius, radius, thickness, slices, stacks, wireFrame);
  return;
}

void cedar::aux::gl::drawEllipse(
                                  double _a,
                                  double _b,
                                  double thickness,
                                  int slices,
                                  int stacks,
                                  bool wireFrame
                                )
{
  float a = static_cast<float>(_a);
  float b = static_cast<float>(_b);
  float t = static_cast<float>(thickness);
  //  double t = thickness;
  float d = 4.0/3.0*(sqrt(2.0)-1);


// approximates the ellipse with 16 3rd order Bezier surfaces, which is not perfect, but almost

  // first quarter
  GLfloat I_1[4][4][3] =
  {
    {{b, 0.0, t}, {b+d*t, 0.0, t},    {b+t, 0.0, d*t},    {b+t, 0.0, 0.0}},
    {{b, d*a, t}, {b+d*t, d*(a+d*t), t}, {b+t, d*(a+t), d*t}, {b+t, d*(a+t), 0.0}},
    {{d*b, a, t}, {d*(b+d*t), a+d*t, t}, {d*(b+t), a+t, d*t}, {d*(b+t), a+t, 0.0}},
    {{0.0, a, t}, {0.0, a+d*t, t},    {0.0, a+t, d*t},    {0.0, a+t, 0.0}}
  };
  GLfloat I_2[4][4][3] =
  {
    {{0.0, a, t}, {0.0, a-d*t, t},    {0.0, a-t, d*t},    {0.0, a-t, 0.0}},
    {{d*b, a, t}, {d*(b-d*t), a-d*t, t}, {d*(b-t), a-t, d*t}, {d*(b-t), a-t, 0.0}},
    {{b, d*a, t}, {b-d*t, d*(a-d*t), t}, {b-t, d*(a-t), d*t}, {b-t, d*(a-t), 0.0}},
    {{b, 0.0, t}, {b-d*t, 0.0, t},    {b-t, 0.0, d*t},    {b-t, 0.0, 0.0}}
  };
  GLfloat I_3[4][4][3] =
  {
    {{b, 0.0, -t}, {b-d*t, 0.0, -t},    {b-t, 0.0, -d*t},    {b-t, 0.0, 0.0}},
    {{b, d*a, -t}, {b-d*t, d*(a-d*t), -t}, {b-t, d*(a-t), -d*t}, {b-t, d*(a-t), 0.0}},
    {{d*b, a, -t}, {d*(b-d*t), a-d*t, -t}, {d*(b-t), a-t, -d*t}, {d*(b-t), a-t, 0.0}},
    {{0.0, a, -t}, {0.0, a-d*t, -t},    {0.0, a-t, -d*t},    {0.0, a-t, 0.0}}
  };
  GLfloat I_4[4][4][3] =
  {
    {{0.0, a, -t}, {0.0, a+d*t, -t},    {0.0, a+t, -d*t},    {0.0, a+t, -0.0}},
    {{d*b, a, -t}, {d*(b+d*t), a+d*t, -t}, {d*(b+t), a+t, -d*t}, {d*(b+t), a+t, -0.0}},
    {{b, d*a, -t}, {b+d*t, d*(a+d*t), -t}, {b+t, d*(a+t), -d*t}, {b+t, d*(a+t), -0.0}},
    {{b, 0.0, -t}, {b+d*t, 0.0, -t},    {b+t, 0.0, -d*t},    {b+t, 0.0, -0.0}}
  };

  // second quarter
  GLfloat II_1[4][4][3] =
  {
    {{0.0, a, t}, {0.0, a+d*t, t},    {0.0, a+t, d*t},    {0.0, a+t, 0.0}},
    {{-d*b, a, t}, {-d*(b+d*t), a+d*t, t}, {-d*(b+t), a+t, d*t}, {-d*(b+t), a+t, 0.0}},
    {{-b, d*a, t}, {-b-d*t, d*(a+d*t), t}, {-b-t, d*(a+t), d*t}, {-b-t, d*(a+t), 0.0}},
    {{-b, 0.0, t}, {-b-d*t, 0.0, t},    {-b-t, 0.0, d*t},    {-b-t, 0.0, 0.0}}
  };
  GLfloat II_2[4][4][3] =
  {
    {{-b, 0.0, t}, {-b+d*t, 0.0, t},    {-b+t, 0.0, d*t},    {-b+t, 0.0, 0.0}},
    {{-b, d*a, t}, {-b+d*t, d*(a-d*t), t}, {-b+t, d*(a-t), d*t}, {-b+t, d*(a-t), 0.0}},
    {{-d*b, a, t}, {-d*(b-d*t), a-d*t, t}, {-d*(b-t), a-t, d*t}, {-d*(b-t), a-t, 0.0}},
    {{0.0, a, t}, {0.0, a-d*t, t},    {0.0, a-t, d*t},    {0.0, a-t, 0.0}}
  };
  GLfloat II_3[4][4][3] =
  {
    {{0.0, a, -t}, {0.0, a-d*t, -t},    {0.0, a-t, -d*t},    {0.0, a-t, 0.0}},
    {{-d*b, a, -t}, {-d*(b-d*t), a-d*t, -t}, {-d*(b-t), a-t, -d*t}, {-d*(b-t), a-t, 0.0}},
    {{-b, d*a, -t}, {-b+d*t, d*(a-d*t), -t}, {-b+t, d*(a-t), -d*t}, {-b+t, d*(a-t), 0.0}},
    {{-b, 0.0, -t}, {-b+d*t, 0.0, -t},    {-b+t, 0.0, -d*t},    {-b+t, 0.0, 0.0}}
  };
  GLfloat II_4[4][4][3] =
  {
    {{-b, 0.0, -t}, {-b-d*t, 0.0, -t},    {-b-t, 0.0, -d*t},    {-b-t, 0.0, 0.0}},
    {{-b, d*a, -t}, {-b-d*t, d*(a+d*t), -t}, {-b-t, d*(a+t), -d*t}, {-b-t, d*(a+t), 0.0}},
    {{-d*b, a, -t}, {-d*(b+d*t), a+d*t, -t}, {-d*(b+t), a+t, -d*t}, {-d*(b+t), a+t, 0.0}},
    {{0.0, a, -t}, {0.0, a+d*t, -t},    {0.0, a+t, -d*t},    {0.0, a+t, 0.0}}
  };

  // third quarter
  GLfloat III_1[4][4][3] =
  {
    {{0.0, -a, t}, {0.0, -a-d*t, t},    {0.0, -a-t, d*t},    {0.0, -a-t, 0.0}},
    {{d*b, -a, t}, {d*(b+d*t), -a-d*t, t}, {d*(b+t), -a-t, d*t}, {d*(b+t), -a-t, 0.0}},
    {{b, -d*a, t}, {b+d*t, -d*(a+d*t), t}, {b+t, -d*(a+t), d*t}, {b+t, -d*(a+t), 0.0}},
    {{b, 0.0, t}, {b+d*t, 0.0, t},    {b+t, 0.0, d*t},    {b+t, 0.0, 0.0}}
  };
  GLfloat III_2[4][4][3] =
  {
    {{b, 0.0, t}, {b-d*t, 0.0, t},    {b-t, 0.0, d*t},    {b-t, 0.0, 0.0}},
    {{b, -d*a, t}, {b-d*t, -d*(a-d*t), t}, {b-t, -d*(a-t), d*t}, {b-t, -d*(a-t), 0.0}},
    {{d*b, -a, t}, {d*(b-d*t), -a+d*t, t}, {d*(b-t), -a+t, d*t}, {d*(b-t), -a+t, 0.0}},
    {{0.0, -a, t}, {0.0, -a+d*t, t},    {0.0, -a+t, d*t},    {0.0, -a+t, 0.0}}
  };
  GLfloat III_3[4][4][3] =
  {
    {{0.0, -a, -t}, {0.0, -a+d*t, -t},    {0.0, -a+t, -d*t},    {0.0, -a+t, 0.0}},
    {{d*b, -a, -t}, {d*(b-d*t), -a+d*t, -t}, {d*(b-t), -a+t, -d*t}, {d*(b-t), -a+t, 0.0}},
    {{b, -d*a, -t}, {b-d*t, -d*(a-d*t), -t}, {b-t, -d*(a-t), -d*t}, {b-t, -d*(a-t), 0.0}},
    {{b, 0.0, -t}, {b-d*t, 0.0, -t},    {b-t, 0.0, -d*t},    {b-t, 0.0, 0.0}}
  };
  GLfloat III_4[4][4][3] =
  {
    {{b, 0.0, -t}, {b+d*t, 0.0, -t},    {b+t, 0.0, -d*t},    {b+t, 0.0, -0.0}},
    {{b, -d*a, -t}, {b+d*t, -d*(a+d*t), -t}, {b+t, -d*(a+t), -d*t}, {b+t, -d*(a+t), -0.0}},
    {{d*b, -a, -t}, {d*(b+d*t), -a-d*t, -t}, {d*(b+t), -a-t, -d*t}, {d*(b+t), -a-t, -0.0}},
    {{0.0, -a, -t}, {0.0, -a-d*t, -t},    {0.0, -a-t, -d*t},    {0.0, -a-t, -0.0}}
  };

  // fourth quarter
  GLfloat IV_1[4][4][3] =
  {
    {{-b, 0.0, t}, {-b-d*t, 0.0, t},    {-b-t, 0.0, d*t},    {-b-t, 0.0, 0.0}},
    {{-b, -d*a, t}, {-b-d*t, -d*(a+d*t), t}, {-b-t, -d*(a+t), d*t}, {-b-t, -d*(a+t), 0.0}},
    {{-d*b, -a, t}, {-d*(b+d*t), -a-d*t, t}, {-d*(b+t), -a-t, d*t}, {-d*(b+t), -a-t, 0.0}},
    {{0.0, -a, t}, {0.0, -a-d*t, t},    {0.0, -a-t, d*t},    {0.0, -a-t, 0.0}}
  };
  GLfloat IV_2[4][4][3] =
  {
    {{0.0, -a, t}, {0.0, -a+d*t, t},    {0.0, -a+t, d*t},    {0.0, -a+t, 0.0}},
    {{-d*b, -a, t}, {-d*(b-d*t), -a+d*t, t}, {-d*(b-t), -a+t, d*t}, {-d*(b-t), -a+t, 0.0}},
    {{-b, -d*a, t}, {-b+d*t, -d*(a-d*t), t}, {-b+t, -d*(a-t), d*t}, {-b+t, -d*(a-t), 0.0}},
    {{-b, 0.0, t}, {-b+d*t, 0.0, t},    {-b+t, 0.0, d*t},    {-b+t, 0.0, 0.0}}
  };
  GLfloat IV_3[4][4][3] =
  {
    {{-b, 0.0, -t}, {-b+d*t, 0.0, -t},    {-b+t, 0.0, -d*t},    {-b+t, 0.0, 0.0}},
    {{-b, -d*a, -t}, {-b+d*t, -d*(a-d*t), -t}, {-b+t, -d*(a-t), -d*t}, {-b+t, -d*(a-t), 0.0}},
    {{-d*b, -a, -t}, {-d*(b-d*t), -a+d*t, -t}, {-d*(b-t), -a+t, -d*t}, {-d*(b-t), -a+t, 0.0}},
    {{0.0, -a, -t}, {0.0, -a+d*t, -t},    {0.0, -a+t, -d*t},    {0.0, -a+t, 0.0}}
  };
  GLfloat IV_4[4][4][3] =
  {
    {{0.0, -a, -t}, {0.0, -a-d*t, -t},    {0.0, -a-t, -d*t},    {0.0, -a-t, 0.0}},
    {{-d*b, -a, -t}, {-d*(b+d*t), -a-d*t, -t}, {-d*(b+t), -a-t, -d*t}, {-d*(b+t), -a-t, 0.0}},
    {{-b, -d*a, -t}, {-b-d*t, -d*(a+d*t), -t}, {-b-t, -d*(a+t), -d*t}, {-b-t, -d*(a+t), 0.0}},
    {{-b, 0.0, -t}, {-b-d*t, 0.0, -t},    {-b-t, 0.0, -d*t},    {-b-t, 0.0, 0.0}}
  };

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
}

void cedar::aux::gl::drawAxes(double length)
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  cv::Mat o = cv::Mat::zeros(3, 1, CV_64FC1);
  cv::Mat x = cv::Mat::zeros(3, 1, CV_64FC1);
  x.at<double>(0, 0) = length;
  cv::Mat y = cv::Mat::zeros(3, 1, CV_64FC1);
  y.at<double>(1, 0) = length;
  cv::Mat z = cv::Mat::zeros(3, 1, CV_64FC1);
  z.at<double>(2, 0) = length;
  setColor(1, 0, 0);
  drawArrow<double>(o, x, length*0.015, length*0.04, length*0.2, 10);
  setColor(0, 1, 0);
  drawArrow<double>(o, y, length*0.015, length*0.04, length*0.2, 10);
  setColor(0, 0, 1);
  drawArrow<double>(o, z, length*0.015, length*0.04, length*0.2, 10);
}

void cedar::aux::gl::drawCross(double length, double width)
{
  glLineWidth(width);
  glBegin(GL_LINES);
  glVertex3d(-length, 0, 0);
  glVertex3d(length, 0, 0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3d(0, -length, 0);
  glVertex3d(0, length, 0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3d(0, 0, -length);
  glVertex3d(0, 0, length);
  glEnd();
}
