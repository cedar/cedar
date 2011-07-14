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

    File:        MatrixPlot.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/gui/MatrixPlot.h"
#include "auxiliaries/gui/MatrixPlot2D.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

Qwt3D::ColorVector cedar::aux::gui::MatrixPlot::mStandardColorVector;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot::MatrixPlot(QWidget *pParent)
:
QWidget(pParent),
mpeLock(NULL),
mpCurrentPlotWidget(NULL)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
}

cedar::aux::gui::MatrixPlot::~MatrixPlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot::display(cv::Mat mat, QReadWriteLock *lock)
{
  this->mMat = mat;
  this->mpeLock = lock;

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  switch (this->mMat.dims)
  {
    case 2:
      if ( (this->mMat.rows == 1 || this->mMat.cols == 1) && this->mMat.rows != this->mMat.cols)
      {
        // TODO
        std::cout << "vector plots aren't implemented yet; sorry :(" << std::endl;
      }
      else
      {
        this->mpCurrentPlotWidget = new cedar::aux::gui::MatrixPlot2D(this->mMat, this->mpeLock);
        this->layout()->addWidget(this->mpCurrentPlotWidget);
      }
      break;

    default:
      CEDAR_THROW(cedar::aux::UnhandledValueException, "The matrix plot widget can not handle a matrix with the given"
                                                       "dimensionality.");
  }
}

const Qwt3D::ColorVector& cedar::aux::gui::MatrixPlot::getStandardColorVector()
{
  if (cedar::aux::gui::MatrixPlot::mStandardColorVector.empty())
  {
    Qwt3D::RGBA rgb;
    rgb.a = 1;
    for(double i = 0; i < 256; i++)
    {
      if(i < 32.0)
      {
        rgb.r = 0.0;
        rgb.g = 0.0;
        rgb.b = 0.5 + 0.5 * i/32.0;
      }
      else if(i < 96.0)
      {
        rgb.r = 0.0;
        rgb.g = (i - 32.0) / 64.0;
        rgb.b = 1;
      }
      else if(i < 160.0)
      {
        rgb.r = (i - 96.0) / 64.0;
        rgb.g = 1.0;
        rgb.b = 1.0 - (i - 96.0) / 64.0;
      }
      else if(i < 224.0)
      {
        rgb.r = 1.0;
        rgb.g = 1.0 - (i - 160.0) / 64.0;
        rgb.b = 0.0;
      }
      else if(i < 256.0)
      {
        rgb.r = 1.0 - (i - 224.0) / 32.0;
        rgb.g = 0.0;
        rgb.b = 0.0;
      }
      cedar::aux::gui::MatrixPlot::mStandardColorVector.push_back(rgb);
    }
  }
  return cedar::aux::gui::MatrixPlot::mStandardColorVector;
}
