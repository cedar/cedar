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
#   Maintainer:  Oliver Lomp,
#                Mathis Richter,
#                Stephan Zibner
#   Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
#                mathis.richter@ini.ruhr-uni-bochum.de,
#                stephan.zibner@ini.ruhr-uni-bochum.de
#   Date:        2011 06 03
#
#   Description:
#
#   Credits:
#
#=======================================================================================================================

cedar_add_library(cedardyn
                  LINK_LIBRARIES
                  cedarproc
                  MOC_HEADERS
                  gui/NeuralFieldView.h
                  fields/NeuralField.h
                  fields/Preshape.h
                  scripts/ExecuteFunctionOnSteps.h
                  steps/HarmonicOscillator.h
                  steps/HebbianConnection.h
                  steps/RateMatrixToSpaceCode.h
                  steps/SpaceCodeToRateMatrix.h
                  steps/RateToSpaceCode.h
                  steps/SerialOrder.h
                  steps/SpaceToRateCode.h
                  steps/TwoStepInput.h
                  steps/LogTimeInterval.h
                  RESOURCES
                  gui/icons/cedar_dynamics_gui_icons.qrc
                  )
