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

cedar_add_library(cedarproc
  LINK_LIBRARIES
  cedaraux
  cedarunits
  MOC_HEADERS
  gui/ArchitectureToolBox.h
  gui/BoolParameter.h
  gui/DoubleParameter.h
  gui/DoubleVectorParameter.h
  gui/EnumParameter.h
  gui/Ide.h
  gui/IdeApplication.h
  gui/Parameter.h
  gui/PluginLoadDialog.h
  gui/PluginManagerDialog.h
  gui/PropertyPane.h
  gui/Scene.h
  gui/StepItem.h
  gui/StepClassList.h
  gui/StringParameter.h
  gui/ToolBox.h
  gui/UIntParameter.h
  gui/UIntVectorParameter.h
  gui/DirectoryParameter.h
  gui/View.h
  source/GaussInput.h
  steps/StaticGain.h
  steps/Projection.h
  LoopedTrigger.h
  Step.h
  RESOURCES
  gui/icons/icons.qrc
  )
