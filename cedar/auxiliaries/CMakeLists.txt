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

if (CEDAR_USE_VTK)
  set(vtk_moc_files gui/VtkLinePlot.h gui/VtkSurfacePlot.h gui/VtkMatrixPlot.h)
else(CEDAR_USE_VTK)
  set(vtk_moc_files "")
endif(CEDAR_USE_VTK)

if (CEDAR_USE_QWT)
  set(qwt_moc_files gui/QwtLinePlot.h gui/MatrixVectorPlot.h)
else(CEDAR_USE_QWT)
  set(qwt_moc_files "")
endif(CEDAR_USE_QWT)

if (CEDAR_USE_QT5)
  set(qt5_moc_files gui/Qt5LinePlot.h gui/Qt5MatrixPlot.h gui/Qt5SurfacePlot.h gui/Qt5SurfacePlotInputHandler.h)
else(CEDAR_USE_QT5)
  set(qt5_moc_files "")
endif(CEDAR_USE_QT5)

set(qcp_moc_files gui/QCLinePlot.h  gui/QCMatrixPlot.h gui/QCMatrixPlot.h gui/qcustomplot.h)

if (CEDAR_USE_QWTPLOT3D)
  set(qwtplot3d_moc_files gui/QwtSurfacePlot.h)
else(CEDAR_USE_QWTPLOT3D)
  set(qwtplot3d_moc_files "")
endif(CEDAR_USE_QWTPLOT3D)

if (CEDAR_USE_FFTW)  
  set(fftw_moc_files convolution/FFTW.h)
else (CEDAR_USE_FFTW)
  set(fftw_moc_files "")
endif (CEDAR_USE_FFTW)

cedar_add_library(cedaraux
  MOC_HEADERS
  ThreadWrapper.h
  detail/ThreadWorker.h
  CallFunctionInThreadALot.h
  detail/CallFunctionInThreadALotWorker.h
  LoopedThread.h
  LocalCoordinateFrame.h
  gl/ObjectVisualization.h
  gui/AccelerationParameter.h
  gui/AngularVelocityParameter.h
  gui/BaseWidget.h
  gui/BoolParameter.h
  gui/BoolVectorParameter.h
  gui/Configurable.h
  gui/DataPlotter.h
  gui/DirectoryParameter.h
  gui/DoubleParameter.h
  gui/DoubleVectorParameter.h
  gui/EnumParameter.h
  gui/ExceptionDialog.h
  gui/FileParameter.h
  gui/FrequencyParameter.h
  gui/HistoryPlot.h
  gui/IgnoreLocaleDoubleSpinBox.h
  gui/ImagePlot.h
  gui/IntParameter.h
  gui/IntVectorParameter.h
  gui/LengthParameter.h
  gui/LocalCoordinateFrameWidget.h
  gui/Log.h
  gui/MatrixPlot.h
  gui/MatrixSlicePlot3D.h
  gui/ObjectParameter.h
  gui/ObjectListParameter.h
  gui/ObjectVisualizationWidget.h
  gui/Parameter.h
  gui/PlaneAngleParameter.h
  gui/PlotInterface.h
  gui/PluginInfoDialog.h
  gui/PluginManagerDialog.h
  gui/PropertyPane.h
  gui/QImagePlot.h
  gui/QwtMatrixPlot.h
  gui/ResourceDialog.h
  gui/SceneControl.h
  gui/SceneWidget.h
  gui/SettingsWidget.h
  gui/StringParameter.h
  gui/StringVectorParameter.h
  gui/ThreadedPlot.h
  gui/TimeParameter.h
  gui/UIntParameter.h
  gui/UIntVectorParameter.h
  gui/VelocityParameter.h
  gui/Viewer.h
  ${qwt_moc_files}
  ${vtk_moc_files}
  ${qwtplot3d_moc_files}
  ${fftw_moc_files}
  ${qt5_moc_files}
  ${qcp_moc_files}
  convolution/Convolution.h
  convolution/KernelList.h
  convolution/Engine.h
  Parameter.h
  ParameterLink.h
  Recorder.h
  Settings.h
  EquationParameterLink.h
  kernel/Separable.h
  kernel/Kernel.h
  kernel/Gauss.h
  kernel/Box.h
  kernel/RotatedGauss.h
  RESOURCES
  gui/icons/cedar_aux_gui_icons.qrc
  LINK_LIBRARIES
  cedarunits
)
