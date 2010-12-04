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

cedar_add_library(cedardev
  robot/KinematicChain.cpp
  robot/KinematicChainModel.cpp
  robot/Component.cpp
  robot/ReferenceGeometry.cpp
  robot/Robot.cpp
  robot/ComponentNotAvailableException.cpp
  robot/SimulatedKinematicChain.cpp
#  robot/gl/KinematicChain.cpp
  HEADERS
  robot/KinematicChain.h
  robot/KinematicChainModel.h
  robot/Component.h
  robot/ReferenceGeometry.h
  robot/Robot.h
  robot/ComponentNotAvailableException.h
  robot/SimulatedKinematicChain.h
#  robot/gl/KinematicChain.h
  MOC_HEADERS
  robot/KinematicChainModel.h
  LINK_LIBRARIES
  cedaraux
)
