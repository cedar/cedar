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

#TODO fix include of kuka/gui/FriStatusWidget if FRI is not installed
cedar_add_library(cedardev
  MOC_HEADERS
  robot/KinematicChainModel.h
  robot/gui/KinematicChainWidget.h
  kteam/gui/EPuckControlWidget.h
  kteam/gui/KTeamPositionControllerWidget.h
  kteam/KTeamPositionController.h
  communication/gui/CommunicationWidget.h
  robot/kuka/gui/FriStatusWidget.h
  LINK_LIBRARIES
  cedaraux
)

# let some of the c++ files know about some cmake variables
# this is used to include or exclude files, e.g. the Amtec related ones

configure_file(
	CMakeDefines.h.in
	"${CEDAR_HOME}/cedar/devices/robot/CMakeDefines.h"
)
