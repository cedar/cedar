#=======================================================================================================================
#
#   Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
# 
#   This file is part of cedar.
#
#   cedar is free software: you can redistribute it and/or modify it under
#   the terms of the GNU Lesser General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   cedar is distributed in the hope that it will be useful, but WITHOUT ANY
#   WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
#   License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with cedar. If not, see <http://www.gnu.org/licenses/>.
#
#=======================================================================================================================
#
#   Institute:   Ruhr-Universitaet Bochum
#                Institut fuer Neuroinformatik
#
#   File:        CMakeLists.txt
#
#   Maintainer:  Mathis Richter
#   Email:       mathis.richter@ini.rub.de
#   Date:        2010 11 29
#
#   Description:
#
#   Credits:
#
#=======================================================================================================================

cedar_add_library(cedaraux
  Base.cpp
  Thread.cpp
  ConfigurationInterface.cpp
  LogFile.cpp
  exceptions/ExceptionBase.cpp
  exceptions/NullPointerException.cpp
  exceptions/IndexOutOfRangeException.cpp
  exceptions/BadConnectionException.cpp
  math/sigmoids.h
  math/sigmoids.cpp
  math/tools.cpp
  math/algebraTools.cpp
  math/coordinateTransformations.cpp
  math/screwCalculus.cpp
  gui/BaseWidget.cpp
  gl/drawShapes.cpp
  gl/Viewer.cpp
  gl/Scene.cpp
  gl/Object.cpp
  gl/Block.cpp
  HEADERS
  Base.h
  Thread.h
  ConfigurationInterface.h
  LogFile.h
  exceptions/ExceptionBase.h
  exceptions/NullPointerException.h
  exceptions/IndexOutOfRangeException.h
  math/Limits.h
  math/tools.h
  math/algebraTools.h
  math/coordinateTransformations.h
  math/screwCalculus.h
  gui/BaseWidget.h
  gl/drawShapes.h
  gl/Viewer.h
  gl/Scene.h
  gl/Object.h
  gl/Block.h
  MOC_HEADERS
  gui/BaseWidget.h
  gl/Viewer.h
  gl/Object.h
)