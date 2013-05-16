/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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


//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace {
  bool registerPlot()
  {
    using cedar::aux::MatData;
    using cedar::aux::gui::VtkSurfacePlot;

    typedef cedar::aux::gui::PlotDeclarationTemplate<MatData, VtkSurfacePlot> DeclarationType;
    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();
    
    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::VtkSurfacePlot::VtkSurfacePlot(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
}

cedar::aux::gui::VtkSurfacePlot::VtkSurfacePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mpLock(new QReadWriteLock())
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::VtkSurfacePlot::~VtkSurfacePlot()
{
  if (mpLock)
  {
    delete mpLock;
  }

  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

#if VTK_MAJOR_VERSION <= 5

  void cedar::aux::gui::VtkSurfacePlot::init()
  {
    QVBoxLayout *p_layout = new QVBoxLayout();
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
    
    mMatData = cedar::aux::MatDataPtr(new MatData(cv::Mat::zeros(300,300, CV_64F)));
    //build the plane
    buildPlane(300,300);

    //prepare warp
    mpWarper = vtkSmartPointer<vtkWarpScalar>::New();
    mpWarper->SetInput(mpData);
    mpWarper->XYPlaneOn();
    mpWarper->SetScaleFactor(0.0);
   
    //prepare datamapper
    mpMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mpMapper->SetInputConnection(mpWarper->GetOutputPort());

    // Setup the view
    vtkSmartPointer<vtkActor> pActor = vtkSmartPointer<vtkActor>::New();
    // link the datamapper with the actor
    pActor->SetMapper(mpMapper);

    mpRenderer = vtkSmartPointer<vtkRenderer>::New();
    mpRenderWindow = mpVtkWidget->GetRenderWindow();
    mpRenderWindow->AddRenderer(mpRenderer);
    
    // add the actor (the plane) to the view
    mpRenderer->AddActor(pActor);
    mpRenderer->SetBackground(.3, .3, .3);
    
    mpRenderWindow->Render();
    //setup plot interaction & set interactor style to trackball (the way you move in the plot with the mouse)
    mpVtkWidget->GetInteractor()->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
    mpRenderer->GetActiveCamera()->SetFocalPoint(0,0,0);

    // create and add CubeAxes to the plot
    vtkSmartPointer<vtkCubeAxesActor2D> pAxes = vtkSmartPointer<vtkCubeAxesActor2D>::New();
    pAxes->SetInput(mpWarper->GetOutput());
    pAxes->SetFontFactor(2.0);
    //axes shouldn't jump around
    pAxes->SetInertia(0);
    pAxes->SetFlyModeToNone();
    pAxes->SetCamera(mpRenderer->GetActiveCamera());
    mpRenderer->AddViewProp(pAxes);

    mpWorkerThread = new QThread();

    mConversionWorker = cedar::aux::gui::detail::VtkSurfacePlotWorkerPtr(new cedar::aux::gui::detail::VtkSurfacePlotWorker(this));
    mConversionWorker->moveToThread(mpWorkerThread);

    //!@todo Add possibility to change priority
    this->mpWorkerThread->start(QThread::LowPriority);

    QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this,                    SIGNAL(dataChanged()));
    QObject::connect(this,                    SIGNAL(convert()),     mConversionWorker.get(), SLOT(convert()));
    QObject::connect(mConversionWorker.get(), SIGNAL(done()),        this,                    SLOT(conversionDone()));
  }

  void cedar::aux::gui::VtkSurfacePlot::plot(cedar::aux::ConstDataPtr data, const std::string&)
  {
    this->mMatData = boost::shared_dynamic_cast<cedar::aux::ConstMatData>(data);

    if (!this->mMatData)
    {
      CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                  "Could not cast to cedar::aux::MatData in cedar::aux::gui::SurfacePlot::plot.");
    }
    buildPlane(static_cast<unsigned int>(mMatData->getData().rows), static_cast<unsigned int>(mMatData->getData().cols));
    this->mpRenderer->GetActiveCamera()->SetFocalPoint(mMatData->getData().rows / 2,mMatData->getData().cols / 2, 0);
    this->mpRenderer->GetActiveCamera()->SetPosition(mMatData->getData().rows / 2, -2 * mMatData->getData().cols, 8 * cedar::aux::math::max(mMatData->getData()));
    this->mpRenderer->GetActiveCamera()->SetClippingRange(0.1, mMatData->getData().cols * 3);
    this->startTimer(60); //!@todo make the refresh time configurable.
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

    if ((data.rows-1) != mpPlot->mpPlane->GetYResolution() || (data.cols-1) != mpPlot->mpPlane->GetXResolution())
    {
      mpPlot->buildPlane(static_cast<unsigned int>(data.rows), static_cast<unsigned int>(data.cols));
      mpPlot->mpRenderer->GetActiveCamera()->SetFocalPoint(data.rows / 2,data.cols / 2, 0);
      mpPlot->mpRenderer->GetActiveCamera()->SetPosition(data.rows / 2, -2 * data.cols, 8 * cedar::aux::math::max(data));
      mpPlot->mpRenderer->GetActiveCamera()->SetClippingRange(0.1, data.cols * 3);
    }

    int point_number = 0;
    double point[3];
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
        mpPlot->mpZScalars->SetValue(point_number, data.at<double>(i,j));
        point_number++;
      }
    }
    //color plane
    double tmp[2];
    mpPlot->mpData->GetScalarRange(tmp);
    mpPlot->mpMapper->SetScalarRange(tmp[0],tmp[1]);

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

    QVBoxLayout *p_layout = new QVBoxLayout();
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

    //!@todo Add possibility to change priority
    mpWorkerThread->start(QThread::LowPriority);

    QObject::connect(mConversionWorker.get(), SIGNAL(dataChanged()), this,                    SIGNAL(dataChanged()));
    QObject::connect(this,                    SIGNAL(convert()),     mConversionWorker.get(), SLOT(convert()));
    QObject::connect(mConversionWorker.get(), SIGNAL(done()),        this,                    SLOT(conversionDone()));
  }

  void cedar::aux::gui::VtkSurfacePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
  {
    mMatData = boost::shared_dynamic_cast<cedar::aux::ConstMatData>(data);

    if (!mMatData)
    {
      CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                  "Could not cast to cedar::aux::MatData in cedar::aux::gui::SurfacePlot::plot.");
    }
    mpView->GetRenderWindow()->Render();
    startTimer(60); //!@todo make the refresh time configurable.
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