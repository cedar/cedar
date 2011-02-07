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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 26

 ----- Description: Tests cedar::aux::gl::Chessboard class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/gl/Chessboard.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::aux::gl;
using namespace std;

int main()
{
  LogFile log_file("UnitTestChessboard.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  cedar::aux::ObjectPtr p_object(new cedar::aux::Object());
  Chessboard test_chessboard(p_object);
  Chessboard chessboard(p_object, 1, 2, 3, 5, 500);

  //--------------------------------------------------------------------------------------------------------------------
  // length
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: length" << std::endl;
  chessboard.setLength(10.1);
  if (chessboard.length() != 10.1)
  {
    errors++;
    log_file << "ERROR with setLength() or length()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // width
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: width" << std::endl;
  chessboard.setWidth(11.1);
  if (chessboard.width() != 11.1)
  {
    errors++;
    log_file << "ERROR with setWidth() or width()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // height
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: height" << std::endl;
  chessboard.setHeight(111);
  if (chessboard.height() != 111.0)
  {
    errors++;
    log_file << "ERROR with setHeight() or height()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // rows
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: rows" << std::endl;
  chessboard.setNumberOfRows(32);
  if (chessboard.numberOfRows() != 32)
  {
    errors++;
    log_file << "ERROR with setNumberOfRows() or numberOfRows()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // columns
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: columns" << std::endl;
  chessboard.setNumberOfColumns(32);
  if (chessboard.numberOfColumns() != 32)
  {
    errors++;
    log_file << "ERROR with setNumberOfColumns() or numberOfColumns()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // second color
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: second color" << std::endl;
  chessboard.setSecondColor(0.3, 1, sqrt(2)/2);
  if (
      chessboard.secondColorR() != 0.3
      || chessboard.secondColorG() != 1.0
      || chessboard.secondColorB() != sqrt(2)/2
      )
  {
    errors++;
    log_file << "ERROR with setSecondColor or secondColorR/G/B" << std::endl;
  }
  
  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
