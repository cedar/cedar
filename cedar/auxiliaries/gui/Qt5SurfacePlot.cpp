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

    File:        Qt5SurfacePlot.cpp

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

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QT5_PLOTS
#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Qt5SurfacePlot.h"
#include "cedar/auxiliaries/gui/Qt5SurfacePlotInputHandler.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h"
#include "cedar/auxiliaries/gui/ColorValueRGBA.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"
#include "cedar/auxiliaries/ColorGradient.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/Log.h"
#include <QtDataVisualization/QAbstract3DSeries>
// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QMenu>
#include <QThread>
#include <QtMath>
#include <iostream>
#include <QLinearGradient>
#include <QApplication>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Qt5SurfacePlot::Qt5SurfacePlot(QWidget *pParent)
        :
        cedar::aux::gui::PlotInterface(pParent)
{
  this->init();
}

cedar::aux::gui::Qt5SurfacePlot::Perspective::Perspective(const std::string &name,
                                                          QtDataVisualization::Q3DCamera::CameraPreset camPreset,
                                                          double rotationX, double rotationY)
        :
        mName(name),
        mCamPreset(camPreset)
{
  mRotation[0] = rotationX;
  mRotation[1] = rotationY;
}

cedar::aux::gui::Qt5SurfacePlot::Qt5SurfacePlot(cedar::aux::ConstDataPtr matData, const std::string &title,
                                                QWidget *pParent)
        :
        cedar::aux::gui::PlotInterface(pParent),
        mShowGridLines(false),
        mIsBold(false),
        mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::Qt5SurfacePlot::~Qt5SurfacePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }

  if (mpLock)
  {
    delete mpLock;
  }

  this->deleteArrayData();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Qt5SurfacePlot::deleteArrayData()
{
  this->dataArray = NULL; //todo sinn?
}

void cedar::aux::gui::Qt5SurfacePlot::updateArrayData()
{
  cv::Mat data;
  this->mMatData->lockForRead();
  this->mMatData->getData().convertTo(data, CV_64F);
  this->mMatData->unlock();
  if (cedar::aux::math::getDimensionalityOf(data) != 2) // plot is no longer capable of displaying the data
  {
    emit dataChanged();
    return;
  }
  if (static_cast<size_t>(data.rows) != mDataRows || static_cast<size_t>(data.cols) != mDataCols)
  {
    this->deleteArrayData();
  }
  mDataRows = static_cast<size_t>(data.rows);
  mDataCols = static_cast<size_t>(data.cols);

  this->y_min = std::numeric_limits<double>::max();
  this->y_max = -std::numeric_limits<double>::max();


  this->dataArray = new QtDataVisualization::QSurfaceDataArray();

  this->dataArray->reserve(mDataRows);
  for (size_t i = 0; i < mDataRows; ++i)
  {
    QtDataVisualization::QSurfaceDataRow *newRow = new QtDataVisualization::QSurfaceDataRow(mDataCols);
    for (size_t j = 0; j < mDataCols; ++j)
    {
      double y = data.at<double>(i, j);
      (*newRow)[j].setPosition(QVector3D(i, y, j));
      this->y_min = std::min(this->y_min, y);
      this->y_max = std::max(this->y_max, y);
    }
    *dataArray << newRow;
  }
}

//void cedar::aux::gui::Qt5SurfacePlot::Perspective::applyTo(Qwt3D::Plot3D* pPlot)
//{
//  pPlot->setRotation(mRotation[0], mRotation[1], mRotation[2]);
//  pPlot->setScale(mScale[0], mScale[1], mScale[2]);
//  pPlot->setShift(mShift[0], mShift[1], mShift[2]);
//  pPlot->setZoom(mZoom);
//}

void cedar::aux::gui::Qt5SurfacePlot::plot(cedar::aux::ConstDataPtr data, const std::string & /* title */)
{
  this->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

  if (!this->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::Qt5SurfacePlot::plot.");
  }

  this->startTimer(60);
}

void cedar::aux::gui::Qt5SurfacePlot::init()
{
  this->mPerspectives.push_back(Perspective("DirectlyAbove", QtDataVisualization::Q3DCamera::CameraPresetNone,
                                            270, 90));

  this->mPerspectives.push_back(Perspective("RightHigh", QtDataVisualization::Q3DCamera::CameraPresetRightHigh));

  this->mPerspectives.push_back(
          Perspective("IsometricRightHigh", QtDataVisualization::Q3DCamera::CameraPresetIsometricRightHigh));

  this->mPerspectives.push_back(Perspective("LeftHigh", QtDataVisualization::Q3DCamera::CameraPresetLeftHigh));

  this->mPerspectives.push_back(
          Perspective("IsometricLeftHigh", QtDataVisualization::Q3DCamera::CameraPresetIsometricLeftHigh));



//  // create a new layout for the widget
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  m_graph = new QtDataVisualization::Q3DSurface();
  m_inputHandler = new cedar::aux::gui::Qt5SurfacePlotInputHandler(this);
  m_graph->setActiveInputHandler(m_inputHandler);

  //m_graph->setFlags(m_graph->flags() ^ Qt::FramelessWindowHint);

  QWidget *container = QWidget::createWindowContainer(m_graph);

//  container->setAttribute(Qt::WA_PaintOnScreen);
//  container->setAttribute(Qt::WA_NativeWindow);

  this->resetPerspective();
//  m_graph->scene()->activeCamera()->setCameraPreset(QtDataVisualization::Q3DCamera::CameraPresetDirectlyAbove);
//  m_graph->scene()->activeCamera()->setXRotation(270);
  m_graph->activeTheme()->setAmbientLightStrength(1.0);
  m_graph->activeTheme()->setHighlightLightStrength(0.0);
  m_graph->activeTheme()->setLightStrength(0.0);
  m_graph->activeTheme()->setLabelBorderEnabled(false);
  m_graph->activeTheme()->setGridEnabled(false);
  m_graph->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualityNone);
  auto curFont = m_graph->activeTheme()->font();
  curFont.setPointSize(160);
  curFont.setBold(mIsBold);
  m_graph->activeTheme()->setFont(curFont);
  // m_graph->activeTheme()->setLabelBackgroundEnabled(false);
  // m_graph->activeTheme()->setLabelBackgroundColor(Qt::white);
  // m_graph->activeTheme()->setLabelTextColor(Qt::white);

  //this->m_graph->setMargin(0);
  //container->setMargins(QMargins(1, 1, 1, 1));
  this->layout()->setContentsMargins(0, 0, 0, 0);
  container->setContentsMargins(0, 0, 0, 0);
  //container->setBackgroundRoundness(0);

  this->layout()->addWidget(container);

  //Qt5 Example
  m_graph->setAxisX(new QtDataVisualization::QValue3DAxis);
  m_graph->setAxisY(new QtDataVisualization::QValue3DAxis);
  m_graph->setAxisZ(new QtDataVisualization::QValue3DAxis);

  m_sqrtSinProxy = new QtDataVisualization::QSurfaceDataProxy();
  m_sqrtSinSeries = new QtDataVisualization::QSurface3DSeries(m_sqrtSinProxy);

  m_sqrtSinSeries->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurface); //DrawSurfaceAndWireframe


//  if(m_sqrtSinSeries->isFlatShadingSupported())
//  {
//    std::cout<<"Flat Shading would be Supported, but we do not use it anyway!"<<std::endl;
//  }
  m_sqrtSinSeries->setFlatShadingEnabled(m_sqrtSinSeries->isFlatShadingSupported());


//  std::cout<<"The initial Mesh is: " << m_sqrtSinSeries->mesh()<< std::endl;
  m_sqrtSinSeries->setMesh(QtDataVisualization::QAbstract3DSeries::MeshCube);
//  std::cout<<"The new Mesh is: " << m_sqrtSinSeries->mesh()<< std::endl;

  m_graph->axisX()->setLabelFormat("%.1f");
  m_graph->axisY()->setLabelFormat("%.1f");
  m_graph->axisZ()->setLabelFormat("%.1f");

  m_graph->axisX()->setLabelAutoRotation(30);
  m_graph->axisY()->setLabelAutoRotation(90);
  m_graph->axisZ()->setLabelAutoRotation(30);

  m_graph->addSeries(m_sqrtSinSeries);

  QLinearGradient gr;
//  gr.setColorAt(0.0, Qt::black);
//  gr.setColorAt(0.33, Qt::blue);
//  gr.setColorAt(0.67, Qt::red);
//  gr.setColorAt(1.0, Qt::yellow);


  gr.setColorAt(0.0, Qt::blue);
  gr.setColorAt(0.16, Qt::cyan);
  gr.setColorAt(0.33, Qt::green);
  gr.setColorAt(0.50, Qt::yellow);
  gr.setColorAt(0.67, Qt::red);
  gr.setColorAt(0.999, Qt::darkRed);
  gr.setColorAt(1.0, Qt::darkRed);

  m_graph->seriesList().at(0)->setBaseGradient(gr);
  m_graph->seriesList().at(0)->setColorStyle(QtDataVisualization::Q3DTheme::ColorStyleRangeGradient);

//
//  // create the plot object and add it to this widget
//  this->mpPlot = new Qwt3D::GridPlot(this);
//  p_layout->addWidget(this->mpPlot);
//
//  this->mpPlot->setCoordinateStyle(Qwt3D::BOX);
//  this->resetPerspective();
//  this->showGrid(false);
//
//  // apply the standard color vector
//  this->mpPlot->setDataColor(cedar::aux::ColorGradient::getDefaultPlotColorJet()->toQwt3DStandardColor(50));
//  this->mpPlot->updateGL();
//
//  mDataRows = mDataCols = 0;
//
  this->mpWorkerThread = new QThread();
  mWorker = cedar::aux::gui::detail::Qt5SurfacePlotWorkerPtr(new cedar::aux::gui::detail::Qt5SurfacePlotWorker(this));
  mWorker->moveToThread(this->mpWorkerThread);
  QObject::connect(this, SIGNAL(convert()), mWorker.get(), SLOT(convert()));//,Qt::DirectConnection)
  QObject::connect(mWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));//,Qt::DirectConnection

  mpWorkerThread->start(QThread::LowPriority);
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::Qt5SurfacePlotWorker::convert()
{
  this->mpPlot->updateArrayData();
  emit done();
}
//!@endcond

void cedar::aux::gui::Qt5SurfacePlot::conversionDone()
{
  QReadLocker locker(this->mpLock);

  m_graph->axisX()->setRange(-0.001f, mDataRows + 0.001f);
  m_graph->axisY()->setRange(this->y_min - 0.001f, this->y_max + 0.001f);
  m_graph->axisZ()->setRange(-0.001f, mDataCols + 0.001f);

  m_sqrtSinProxy->resetArray(this->dataArray);
}

void cedar::aux::gui::Qt5SurfacePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (this->isVisible() && this->mMatData)
  {
//    QApplication::processEvents();//QEventLoop::ExcludeUserInputEvents
    emit convert();
  }
}


void cedar::aux::gui::Qt5SurfacePlot::applyLabels()
{
//  if (this->mMatData->hasAnnotation<cedar::aux::annotation::Dimensions>())
//  {
//    cedar::aux::annotation::ConstDimensionsPtr dimensions
//      = this->mMatData->getAnnotation<cedar::aux::annotation::Dimensions>();
//
//    if (dimensions->getDimensionality() == 2)
//    {
//      QString label_x = QString::fromStdString(dimensions->getLabel(0));
//      QString label_y = QString::fromStdString(dimensions->getLabel(1));
//      this->mpPlot->coordinates()->axes[Qwt3D::X1].setLabelString(label_x);
//      this->mpPlot->coordinates()->axes[Qwt3D::X2].setLabelString(label_x);
//      this->mpPlot->coordinates()->axes[Qwt3D::X3].setLabelString(label_x);
//      this->mpPlot->coordinates()->axes[Qwt3D::X4].setLabelString(label_x);
//      this->mpPlot->coordinates()->axes[Qwt3D::Y1].setLabelString(label_y);
//      this->mpPlot->coordinates()->axes[Qwt3D::Y2].setLabelString(label_y);
//      this->mpPlot->coordinates()->axes[Qwt3D::Y3].setLabelString(label_y);
//      this->mpPlot->coordinates()->axes[Qwt3D::Y4].setLabelString(label_y);
//    }
//    else
//    {
//      cedar::aux::LogSingleton::getInstance()->debugMessage
//      (
//        "Wrong number of entries in dimension annotations.",
//        "cedar::aux::gui::Qt5SurfacePlot::plot"
//      );
//    }
//  }
}

void cedar::aux::gui::Qt5SurfacePlot::resetPerspective(size_t perspectiveIndex)
{

//  CEDAR_ASSERT(perspectiveIndex < this->mPerspectives.size());
  Perspective &perspective = this->mPerspectives.at(perspectiveIndex);
// std::cout <<" Resize Perspective! Size_T : "<< perspectiveIndex << " Name: "<<  << " X Rotation: " << perspective.getRotationX() << " Y Rotation: "<< perspective.getRotationY() << std::endl;
// m_graph->scene()->activeCamera()->setCameraPreset(perspective.getPreset());

  if (perspective.getPreset() != QtDataVisualization::Q3DCamera::CameraPresetNone)
  {
    m_graph->scene()->activeCamera()->setCameraPreset(perspective.getPreset());
  } else
  {
    m_graph->scene()->activeCamera()->setXRotation(perspective.getRotationX());
    m_graph->scene()->activeCamera()->setYRotation(perspective.getRotationY());
  }
//  std::cout << " Set Perspective:" << perspective.getName() << "CameraRotation X: "
//            << m_graph->scene()->activeCamera()->xRotation() << " ,Y: " << m_graph->scene()->activeCamera()->yRotation()
//            << std::endl;
}

void cedar::aux::gui::Qt5SurfacePlot::showGrid(bool show)
{
  this->mShowGridLines = show;
  if (show)
  {
    m_sqrtSinSeries->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurfaceAndWireframe);
  } else
  {
    m_sqrtSinSeries->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurface);
  }
}

void cedar::aux::gui::Qt5SurfacePlot::setBold(bool isBold)
{
  this->mIsBold = isBold;
  auto curFont = m_graph->activeTheme()->font();
  curFont.setBold(isBold);
  m_graph->activeTheme()->setFont(curFont);
}

//void cedar::aux::gui::Qt5SurfacePlot::contextMenuEvent(QContextMenuEvent *pEvent)
//{
//  QMenu menu(this);
//
//  QAction *p_reset_perspective = menu.addAction("reset perspective");
//
//  QMenu *p_perspectives = menu.addMenu("set perspective");
//  for (size_t i = 0; i < this->mPerspectives.size(); ++i)
//  {
//    QAction *p_action = p_perspectives->addAction(this->mPerspectives.at(i).getName().c_str());
//    p_action->setData(QVariant(static_cast<uint>(i)));
//  }
//
//  menu.addSeparator();
//
//  QAction *p_show_grid = menu.addAction("show grid");
//  p_show_grid->setCheckable(true);
//  p_show_grid->setChecked(this->mShowGridLines);
//
//  QAction *p_action = menu.exec(pEvent->globalPos());
//
//  if (p_action != NULL)
//  {
//    if (p_action == p_reset_perspective)
//    {
//      this->resetPerspective();
//    } else if (p_action->parentWidget() == p_perspectives)
//    {
//      size_t index = static_cast<size_t>(p_action->data().toUInt());
//      this->resetPerspective(index);
//    } else if (p_action == p_show_grid)
//    {
//      this->showGrid(!this->mShowGridLines);
//    }
//  }
//}
#endif // CEDAR_USE_QT5_PLOTS