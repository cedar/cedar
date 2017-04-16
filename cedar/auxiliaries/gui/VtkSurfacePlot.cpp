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

    File:        VtkSurfacePlot.cpp

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.ruhr-uni-bochum.de
    Date:        2012 11 07

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_VTK

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/VtkSurfacePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QThread>
#include <iostream>

// VTK INCLUDES
#if VTK_MAJOR_VERSION <= 5
  #include <vtkPointData.h>
  #include <vtkPolyDataMapper.h>
  #include <vtkTransform.h>
  #include <vtkCamera.h>
  #include <vtkCubeAxesActor2D.h>
  #include <vtkInteractorStyleTrackballCamera.h>
  #include <vtkInteractorStyleTrackball.h>
  #include <vtkProperty2D.h>
  #include <vtkProperty.h>
  #include <vtkLookupTable.h>
  #include <vtkTextProperty.h>
#else // VTK_MAJOR_VERSION <= 5
  #include <vtkContextScene.h>
  #include <vtkVector.h>
#endif // VTK_MAJOR_VERSION <= 5

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::VtkSurfacePlot::VtkSurfacePlot(QWidget* pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::VtkSurfacePlot::VtkSurfacePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::VtkSurfacePlot::~VtkSurfacePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }
  // delete lock AFTER waiting for conversion worker to finish
  if (mpLock)
  {
    delete mpLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

#if VTK_MAJOR_VERSION <= 5

  void cedar::aux::gui::VtkSurfacePlot::init()
  {
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(p_layout);

    mpVtkWidget = new QVTKWidget;
    //add that widget to the layout
    this->layout()->addWidget(mpVtkWidget);
    this->layout()->update();

    mpPlane = vtkSmartPointer<vtkPlaneSource>::New();
    mpPlane->SetCenter(1.0,0.0,0.0);
    mpPlane->SetNormal(1.0,0.0,1.0);
    mpPlane->Update();
    
    mpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    vtkSmartPointer<vtkTransform> pTransform = vtkSmartPointer<vtkTransform>::New();
    //choose transform operation to be as simple as possible
    pTransform->Scale(1.0,1.0,1.0);
    mpTransformFilter->SetInputConnection(mpPlane->GetOutputPort());
    mpTransformFilter->SetTransform(pTransform);
    mpTransformFilter->Update();
    
    mpData = vtkSmartPointer<vtkPolyData>::New();
    
    mMatData = cedar::aux::MatDataPtr(new MatData(cv::Mat::zeros(3,3, CV_64F)));
    //build the plane
    buildPlane(3,3);

    //prepare warp
    mpWarper = vtkSmartPointer<vtkWarpScalar>::New();
    mpWarper->SetInput(mpData);
    mpWarper->XYPlaneOn();
    mpWarper->SetScaleFactor(0.0);
   
    //prepare datamapper
    mpMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mpMapper->SetInputConnection(mpWarper->GetOutputPort());

    // create lookup table for coloring
    vtkLookupTable* pLut = vtkLookupTable::New();
    pLut->SetHueRange(0.7, 0);
    mpMapper->SetLookupTable(pLut);

    // Setup the view
    vtkSmartPointer<vtkActor> pActor = vtkSmartPointer<vtkActor>::New();
    // link the datamapper with the actor
    pActor->SetMapper(mpMapper);
    // we don't want directional light
    pActor->GetProperty()->SetAmbient(1.0);
    pActor->GetProperty()->SetDiffuse(0.0);
    pActor->GetProperty()->SetSpecular(0.0);

    mpRenderer = vtkSmartPointer<vtkRenderer>::New();
    mpRenderWindow = mpVtkWidget->GetRenderWindow();
    mpRenderWindow->AddRenderer(mpRenderer);
    
    // add the actor (the plane) to the view
    mpRenderer->AddActor(pActor);
    mpRenderer->SetBackground(1.0, 1.0, 1.0);
    
    mpRenderWindow->Render();
    //setup plot interaction & set interactor style to trackball (the way you move in the plot with the mouse)
    mpVtkWidget->GetInteractor()->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
    mpRenderer->GetActiveCamera()->SetFocalPoint(0,0,0);

    // create and add CubeAxes to the plot
    vtkSmartPointer<vtkCubeAxesActor2D> pAxes = vtkSmartPointer<vtkCubeAxesActor2D>::New();
    pAxes->SetInput(mpWarper->GetOutput());
    pAxes->SetFontFactor(1.0);
    // axis & labels should be black
    pAxes->GetProperty()->SetColor(.0, .0, .0);
    pAxes->GetAxisLabelTextProperty()->SetColor(.0, .0, .0);
    pAxes->GetAxisTitleTextProperty()->SetColor(.0, .0, .0);
    //axes shouldn't jump around
    pAxes->SetInertia(0);
    pAxes->SetFlyModeToNone();
    pAxes->SetCamera(mpRenderer->GetActiveCamera());
    mpRenderer->AddViewProp(pAxes);

    mpWorkerThread = new QThread();

    mConversionWorker = cedar::aux::gui::detail::VtkSurfacePlotWorkerPtr(new cedar::aux::gui::detail::VtkSurfacePlotWorker(this));
    mConversionWorker->moveToThread(mpWorkerThread);

    this->mpWorkerThread->start(QThread::LowPriority);

    QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this,                    SIGNAL(dataChanged()));
    QObject::connect(this,                    SIGNAL(convert()),     mConversionWorker.get(), SLOT(convert()));
    QObject::connect(mConversionWorker.get(), SIGNAL(done()),        this,                    SLOT(conversionDone()));
  }

  void cedar::aux::gui::VtkSurfacePlot::plot(cedar::aux::ConstDataPtr data, const std::string&)
  {
    this->mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

    if (!this->mMatData)
    {
      CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Could not cast to cedar::aux::MatData.");
    }
    buildPlane(static_cast<unsigned int>(mMatData->getData().cols), static_cast<unsigned int>(mMatData->getData().rows));
    setupCamera(this->mpRenderer->GetActiveCamera(), mMatData->getData());
    this->startTimer(60);
  }



  void cedar::aux::gui::VtkSurfacePlot::buildPlane(unsigned int x_size, unsigned int y_size)
  {
    // create a plane
    mpPlane->SetResolution(x_size-1, y_size-1);
    mpTransformFilter->Update();
    //prepare plane for warping + coloring
    int numPts = mpTransformFilter->GetOutput()->GetNumberOfPoints();

    mpPlanePoints = vtkSmartPointer<vtkPoints>::New();
    mpZScalars = vtkSmartPointer<vtkDoubleArray>::New();

    mpPlanePoints->SetNumberOfPoints(numPts);
    mpZScalars->SetNumberOfTuples(numPts);

    mpData->CopyStructure(mpTransformFilter->GetOutput());
    mpData->SetPoints(mpPlanePoints);
    mpData->GetPointData()->SetScalars(mpZScalars);
  }

  void cedar::aux::gui::VtkSurfacePlot::setupCamera(vtkCamera* pCamera, const cv::Mat& data)
  {
    // the center of the plot should be our focal point
    pCamera->SetFocalPoint(data.cols / 2,data.rows / 2, 0);
    // we assume a camera view angle of 30 degrees (this is the standard camera view angle in vtk)
    CEDAR_DEBUG_ASSERT(pCamera->GetViewAngle() == 30.0);

    // dd is the minimum distance to view the diagonal d of the bounding box; 0.2618 rad == 15 degree
    double dd = sqrt(pow(cedar::aux::math::max(data), 2.0) + pow(data.cols, 2.0) + pow(data.rows, 2.0)) / (2.0 * tan(0.2618));
    // h is a suitable height to view the plot, it is 1.5 times the length of a cathetus in a isoscele-right triangle (45-45-90)
    double h = sqrt(1.5 * dd / 2.0);
    // point A of that isoscele-right triangle lies in the center of the bounding box, thus the coordinates for the camera are...
    double z = 3.0 * h + (cedar::aux::math::max(data));
    double y = -data.rows - h * 3.0;
    pCamera->SetPosition(data.cols/2.0, y, z);
    pCamera->SetClippingRange(0.1, 1.5 * dd);
  }

  //!@cond SKIPPED_DOCUMENTATION
  void cedar::aux::gui::detail::VtkSurfacePlotWorker::convert()
  {
    QWriteLocker plot_locker(mpPlot->mpLock);

    cv::Mat data;
    mpPlot->mMatData->lockForRead();
    mpPlot->mMatData->getData().convertTo(data, CV_64F);
    mpPlot->mMatData->unlock();
    if (cedar::aux::math::getDimensionalityOf(data) != 2) // plot is no longer capable of displaying the data
    {
      emit dataChanged();
      return;
    }

    if ((data.cols-1) != mpPlot->mpPlane->GetXResolution() || (data.rows-1) != mpPlot->mpPlane->GetYResolution())
    {
      mpPlot->buildPlane(static_cast<unsigned int>(data.cols), static_cast<unsigned int>(data.rows));
      mpPlot->setupCamera(mpPlot->mpRenderer->GetActiveCamera(), data);
    }

    int point_number = 0;
    double point[3];
    double scalar_range[2] = {data.at<double>(0, 0), data.at<double>(0, 0)};
    for(int i=0; i<data.rows; i++)
    {
      for(int j=0; j<data.cols; j++)
      {
        //InsertNextPoint(x,y,z)
        point[0] = j;
        point[1] = i;
        point[2] = data.at<double>(i,j);
        // create the point
        mpPlot->mpPlanePoints->SetPoint(point_number, point);
        // store the z value in z_scalars for coloring
        mpPlot->mpZScalars->SetValue(point_number, point[2]);
        point_number++;

        // find out the scalar range
        if(scalar_range[0] > point[2])
        {
          scalar_range[0] = point[2];
        }
        else if(scalar_range[1] < point[2])
        {
          scalar_range[1] = point[2];
        }
      }
    }
    //color plane
    mpPlot->mpMapper->SetScalarRange(scalar_range);

    plot_locker.unlock();
    emit done();
  }

  void cedar::aux::gui::VtkSurfacePlot::conversionDone()
  {
    QReadLocker locker(this->mpLock);
    // render the plot
    mpWarper->Modified();
    mpMapper->Modified();
    mpRenderWindow->Render();
    locker.unlock();
  }
  //!@endcond

#else
  // VTK 6 Version
  void cedar::aux::gui::VtkSurfacePlot::init()
  {
    mpVtkWidget = new QVTKWidget;

    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(p_layout);

    mpView = vtkSmartPointer<vtkContextView>::New();
    mpView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    mpView->SetInteractor(mpVtkWidget->GetInteractor());
    mpVtkWidget->SetRenderWindow(mpView->GetRenderWindow());
    mpChart = vtkSmartPointer<vtkChartXYZ>::New();
    mpSurface = vtkSmartPointer<vtkPlotSurface>::New();
    mpTable = vtkSmartPointer<vtkTable>::New();

    mpView->GetScene()->AddItem(mpChart.GetPointer());
    mpChart->SetGeometry(vtkRectf(75.0, 20.0, 250, 260));

    // Set up the surface plot we wish to visualize and add it to the chart.
    mpSurface->SetXRange(0, 0);
    mpSurface->SetYRange(0, 0);
    mpSurface->SetInputData(mpTable.GetPointer());
    mpChart->AddPlot(mpSurface.GetPointer());

    // this crashes.. mpView->GetRenderWindow()->SetMultiSamples(0);
    //mpView->GetInteractor()->Initialize();

    //add that widget to the layout
    layout()->addWidget(mpVtkWidget);

    mpWorkerThread = new QThread();

    mConversionWorker = cedar::aux::gui::detail::VtkSurfacePlotWorkerPtr(new cedar::aux::gui::detail::VtkSurfacePlotWorker(this));
    mConversionWorker->moveToThread(mpWorkerThread);

    mpWorkerThread->start(QThread::LowPriority);

    QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this,                    SIGNAL(dataChanged()));
    QObject::connect(this,                    SIGNAL(convert()),     mConversionWorker.get(), SLOT(convert()));
    QObject::connect(mConversionWorker.get(), SIGNAL(done()),        this,                    SLOT(conversionDone()));
  }

  void cedar::aux::gui::VtkSurfacePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
  {
    mMatData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);

    if (!mMatData)
    {
      CEDAR_THROW(cedar::aux::gui::InvalidPlotData, "Could not cast to cedar::aux::MatData.");
    }
    mpView->GetRenderWindow()->Render();
    startTimer(60);
  }


  //!@cond SKIPPED_DOCUMENTATION
  void cedar::aux::gui::detail::VtkSurfacePlotWorker::convert()
  {
    QWriteLocker plot_locker(mpPlot->mpLock);

    cv::Mat data;
    mpPlot->mMatData->lockForRead();
    mpPlot->mMatData->getData().convertTo(data, CV_64F);
    mpPlot->mMatData->unlock();
    if (cedar::aux::math::getDimensionalityOf(data) != 2) // plot is no longer capable of displaying the data
    {
      emit dataChanged();
      return;
    }

    //!@todo find out if vtkTable accesses elements column-wise. If so, map col to row for performance, since cv accesses elements row-wise
    if ((data.rows) != mpPlot->mpTable->GetNumberOfRows() || (data.cols) != mpPlot->mpTable->GetNumberOfColumns())
    { // create table
      mpPlot->mpTable = vtkSmartPointer<vtkTable>::New();
      for (int i = 0; i < data.cols; ++i)
      {
        vtkNew<vtkDoubleArray> arr;
        mpPlot->mpTable->AddColumn(arr.GetPointer());
      }
      mpPlot->mpTable->SetNumberOfRows(data.rows);
    }

    // fill table
    for(int i=0; i<data.rows; i++)
    {
      for(int j=0; j<data.cols; j++)
      {
        mpPlot->mpTable->SetValue(i, j, data.at<double>(i,j));
      }
    }
    
    mpPlot->mpSurface->SetInputData(mpPlot->mpTable.GetPointer());
    plot_locker.unlock();
    emit done();
  }

  void cedar::aux::gui::VtkSurfacePlot::conversionDone()
  {
    QReadLocker locker(mpLock);
    // render the plot
    mpView->GetRenderWindow()->Render();
    locker.unlock();
  }
  //!@endcond

#endif // VTK_MAJOR_VERSION

void cedar::aux::gui::VtkSurfacePlot::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!isVisible())
  {
    return;
  }

  emit convert();
}

#endif // CEDAR_USE_VTK
