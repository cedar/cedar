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
#include "auxiliaries/gui/MatrixPlot1D.h"
#include "auxiliaries/gui/MatrixPlot2D.h"
#include "auxiliaries/gui/exceptions.h"
#include "auxiliaries/exceptions.h"
#include "auxiliaries/DataTemplate.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <iostream>

Qwt3D::ColorVector cedar::aux::gui::MatrixPlot::mStandardColorVector;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot::MatrixPlot(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent),
mpCurrentPlotWidget(NULL)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  if (pParent != NULL)
  {
    mWindowTitle = pParent->windowTitle();
  }
}

cedar::aux::gui::MatrixPlot::~MatrixPlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot::display(cedar::aux::DataPtr data)
{
  this->mData= boost::shared_dynamic_cast<cedar::aux::MatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot::display.");
  }

  //!@todo this is buggy: appends a string to the parent's window title over and over again
//  if (QWidget *p_parent = dynamic_cast<QWidget*>(this->parent()))
//  {
//    QString add = QString(" [%1 x %2]").arg(this->mData->getData().rows).arg(this->mData->getData().cols);
//    p_parent->setWindowTitle(mWindowTitle + add);
//  }

  if (this->mpCurrentPlotWidget)
  {
    delete this->mpCurrentPlotWidget;
    this->mpCurrentPlotWidget = NULL;
  }

  cv::Mat& mat = this->mData->getData();

  switch (mat.dims)
  {
    case 2:
      if ( (mat.rows == 1 || mat.cols == 1) && mat.rows != mat.cols)
      {
        this->mpCurrentPlotWidget = new cedar::aux::gui::MatrixPlot1D(this->mData);
      }
      else
      {
        this->mpCurrentPlotWidget = new cedar::aux::gui::MatrixPlot2D(this->mData);
      }
      this->layout()->addWidget(this->mpCurrentPlotWidget);
      connect(this->mpCurrentPlotWidget, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
      break;

    default:
    {
      std::string message = "The matrix plot widget can not handle a matrix with the given dimensionality (";
      message += QString("%1).").arg(mat.dims).toStdString(); //!@todo replace QString with proper aux function.
      message += "\nPress here to refresh the plot after you have changed the dimensionality.";
      this->mpCurrentPlotWidget = new QPushButton(QString::fromStdString(message));
      this->layout()->addWidget(this->mpCurrentPlotWidget);
      connect(this->mpCurrentPlotWidget, SIGNAL(pressed()), this, SIGNAL(dataChanged()));
    }
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
