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

    File:        gui_MatrixPlotsBasics.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 10 29

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"
#include "cedar/auxiliaries/gui/HistoryPlot0D.h"
#include "cedar/auxiliaries/gui/LinePlot.h"
#include "cedar/auxiliaries/gui/SurfacePlot.h"
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

  p_plot->plot(null_mat, "null mat");

  while (QApplication::hasPendingEvents())
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

  errors += testPlottingNullMatrix<cedar::aux::gui::HistoryPlot0D>();
  //!@todo Fix this class for this test.
//  errors += testPlottingNullMatrix<cedar::aux::gui::LinePlot>();
  errors += testPlottingNullMatrix<cedar::aux::gui::SurfacePlot>();

  std::cout << "Done with " << errors << " error(s)." << std::endl;
  return errors;
}
