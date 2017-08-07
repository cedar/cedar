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

    File:        gui_MatrixPlotsBasics.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 10 29

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_QWT
  #include "cedar/auxiliaries/gui/HistoryPlot0D.h"
  #include "cedar/auxiliaries/gui/QwtLinePlot.h"
#endif // CEDAR_USE_QWT
#ifdef CEDAR_USE_QWTPLOT3D
  #include "cedar/auxiliaries/gui/QwtSurfacePlot.h"
#endif // CEDAR_USE_QWTPLOT3D
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <QApplication>
#include <vector>
#include <string>
#include <iostream>


template <class PlotType>
int testPlottingNullMatrix()
{
  int errors = 0;
  PlotType* p_plot = new PlotType();

  std::cout << "Testing plotting of null matrix in " << cedar::aux::typeToString<PlotType>() << "." << std::endl;
  cedar::aux::MatDataPtr null_mat (new cedar::aux::MatData(cv::Mat()));

  try
  {
    p_plot->plot(null_mat, "null mat");
  }
  catch (cedar::aux::gui::InvalidPlotData& e)
  {
    std::cout << "Could not plot: " << std::endl << e.exceptionInfo() << std::endl;
  }

  // maximum amount of events to process (the plot sends timer events)
  unsigned int i = 0;
  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    QApplication::processEvents();
  }

  return errors;
}


int main(int argc, char** argv)
{
  // needs to be created because we deal with widgets here
  QApplication app(argc, argv);

  // the number of errors encountered in this test
  int errors = 0;
#ifdef CEDAR_USE_QWT
  errors += testPlottingNullMatrix<cedar::aux::gui::HistoryPlot0D>();
#endif // CEDAR_USE_QWT
  //!@todo Fix this class for this test.
//  errors += testPlottingNullMatrix<cedar::aux::gui::QwtLinePlot>();
#ifdef CEDAR_USE_QWTPLOT3D
  errors += testPlottingNullMatrix<cedar::aux::gui::QwtSurfacePlot>();
#endif // CEDAR_USE_QWTPLOT3D

  std::cout << "Done with " << errors << " error(s)." << std::endl;
  return errors;
}
