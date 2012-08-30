#=======================================================================================================================
#
#   Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
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

cedar_add_library(cedardev
                  MOC_HEADERS
                  robot/gui/KinematicChainWidget.h
                  robot/gui/KinematicChainMonitorWidget.h
                  robot/gui/KinematicChainCommandWidget.h
                  robot/gui/MountedCameraViewer.h
                  kteam/gui/DriveControlWidget.h
                  communication/gui/CommunicationWidget.h
                  sensors/visual/VideoGrabber.h
                  sensors/visual/PictureGrabber.h
                  sensors/visual/CameraGrabber.h
                  ${kuka_moc_files}
                  LINK_LIBRARIES
                  cedaraux
                  ${linked_libs}
                  )
