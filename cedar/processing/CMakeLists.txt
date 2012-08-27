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
#   Maintainer:  Oliver Lomp,
#                Mathis Richter,
#                Stephan Zibner
#   Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
#                mathis.richter@ini.ruhr-uni-bochum.de,
#                stephan.zibner@ini.ruhr-uni-bochum.de
#   Date:        2011 06 21
#
#   Description:
#
#   Credits:
#
#=======================================================================================================================

if(CEDAR_USE_YARP)
  SET(yarp_moc_headers sinks/NetWriter.h sources/NetReader.h)
else()
  SET(yarp_moc_headers "")
endif()

if(CEDAR_USE_LIB_DC1394)
  SET(libdc_moc_headers sources/Camera.h)
else()
  SET(libdc_moc_headers "")
endif()

cedar_add_library(cedarproc
  LINK_LIBRARIES
  cedaraux
  cedarunits
  cedardev
  MOC_HEADERS
  gui/ArchitectureToolBox.h
  gui/ElementClassList.h
  gui/Ide.h
  gui/IdeApplication.h
  gui/PluginLoadDialog.h
  gui/PluginManagerDialog.h
  gui/ProjectionMappingParameter.h
  gui/Scene.h
  gui/SettingsDialog.h
  gui/StepItem.h
  gui/Network.h
  gui/ToolBox.h
  gui/View.h
  sources/GaussInput.h
  steps/StaticGain.h
  steps/Convolution.h
  steps/ColorConversion.h
  steps/CoordinateTransformation.h
  steps/MatrixTypeConverter.h
  steps/Flip.h
  sources/Picture.h
  sources/Video.h
  sources/GrabberBase.h
  steps/Projection.h
  steps/Resize.h
  Network.h
  Step.h
  ${libdc_moc_headers}
  ${yarp_moc_headers}
  RESOURCES
  gui/icons/icons.qrc
  gui/images/images.qrc
  )
