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

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <vector>
#include <string>
#include <iostream>

#ifndef CEDAR_COMPILER_MSVC

// The class we want to test
#define private public
#include "cedar/auxiliaries/gui/ImagePlot.h"


int test_annotation_with_float(cedar::aux::annotation::ColorSpace::ChannelType type, int matrixType)
{
  std::cout << "Testing matrix of type ";
  switch (matrixType)
  {
    case CV_32F:
      std::cout << "CV_32F";
      break;

    default:
      std::cout << "(unknown)";
  }
  std::cout << " with annotation " << cedar::aux::annotation::ColorSpace::channelTypeToString(type) << std::endl;

  cedar::aux::MatDataPtr data(new cedar::aux::MatData(cv::Mat::ones(40, 40, matrixType)));
  data->setAnnotation(cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(type)));

  auto plot = new cedar::aux::gui::ImagePlot();
  plot->plot(data, "unit test plot");
  plot->doConversion();

  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  delete plot;

  return 0;
}

int test_annotations_with_float(int matrixType)
{
  int errors = 0;

  std::vector<cedar::aux::annotation::ColorSpace::ChannelType> types;
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Red);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Green);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Blue);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Gray);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Hue);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Saturation);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Value);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Alpha);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::Luminance);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::ChromaticRed);
  types.push_back(cedar::aux::annotation::ColorSpace::ChannelType::ChromaticBlue);

  for (size_t i = 0; i < types.size(); ++i)
  {
    auto type = types.at(i);
    test_annotation_with_float(type, matrixType);
  }

  return errors;
}

#endif // CEDAR_COMPILER_MSVC

int main(int argc, char** argv)
{
#ifdef CEDAR_COMPILER_MSVC
  return 0;
#else // CEDAR_COMPILER_MSVC
  // needs to be created because we deal with widgets here
  QApplication app(argc, argv);

  int errors = 0;

  errors += test_annotations_with_float(CV_32F);

  std::cout << "Done with " << errors << " error(s)." << std::endl;

  // the number of errors encountered in this test
  return errors;
#endif // CEDAR_COMPILER_MSVC
}

