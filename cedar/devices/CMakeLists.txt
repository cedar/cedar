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

if (CEDAR_USE_KUKA_LWR)
  set(kuka_moc_files kuka/gui/FriStatusWidget.h)
else(CEDAR_USE_KUKA_LWR)
  set(kuka_moc_files "")
endif(CEDAR_USE_KUKA_LWR)
cedar_add_library(cedardev
                  MOC_HEADERS
                  robot/gui/KinematicChainWidget.h
                  robot/gui/KinematicChainMonitorWidget.h
                  robot/gui/KinematicChainCommandWidget.h
                  kteam/gui/EPuckControlWidget.h
                  communication/gui/CommunicationWidget.h
                  ${kuka_moc_files}
                  LINK_LIBRARIES
                  cedaraux
                  )
