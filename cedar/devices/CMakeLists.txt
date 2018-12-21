#=======================================================================================================================
#
#   Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
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

if (CEDAR_USE_KUKA_LWR)
  set(kuka_moc_files kuka/gui/FriStatusWidget.h)
else(CEDAR_USE_KUKA_LWR)
  set(kuka_moc_files "")
endif(CEDAR_USE_KUKA_LWR)

set (linked_libs "")
if (CEDAR_USE_LIB_DC1394)
	set (linked_libs dc1394)
endif(CEDAR_USE_LIB_DC1394)

if(CEDAR_USE_YARP)
  set(yarp_moc_files YarpKinematicChain.h YarpVehicle.h)
else (CEDAR_USE_YARP)
  set(yarp_moc_files "")
endif(CEDAR_USE_YARP)

cedar_add_library(cedardev
                  MOC_HEADERS
                  Component.h
                  ComponentParameter.h
                  KinematicChain.h
                  SimulatedKinematicChain.h
                  SimulatedVehicle.h
                  SimulatedCamera.h
                  SerialChannel.h
                  Odometry.h
                  Vehicle.h
                  ${yarp_moc_files}
                  gui/KinematicChainWidget.h
                  gui/KinematicChainMonitorWidget.h
                  gui/KinematicChainCommandWidget.h
                  gui/ComponentParameter.h
                  gui/RobotManager.h
                  gui/RobotCard.h
                  gui/MountedCameraViewer.h
                  kteam/gui/DriveControlWidget.h
                  sensors/visual/VideoGrabber.h
                  sensors/visual/PictureGrabber.h
                  sensors/camera/Grabber.h
                  sensors/camera/Channel.h
                  sensors/camera/CamProperty.h
                  sensors/camera/Properties.h
                  ${kuka_moc_files}
				  RESOURCES
				  gui/cedar_dev_gui_icons.qrc
                  LINK_LIBRARIES
                  cedaraux
                  ${linked_libs}
                  )
