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

if (CEDAR_USE_QGLVIEWER)
  set(qglviewer_moc_headers sources/VirtualCamera.h)
else(CEDAR_USE_QGLVIEWER)
  set(qglviewer_moc_headers "")
endif(CEDAR_USE_QGLVIEWER)

if (CEDAR_USE_COPY)
  set(copyscript_moc_headers gui/CoPYWidget.h gui/CoPYObject.h gui/PythonQtConsole.h)
else(CEDAR_USE_COPY)
  set(copyscript_moc_headers "")
endif(CEDAR_USE_COPY)

if (CEDAR_USE_PYTHONSTEP)
  set(pythonstep_moc_headers steps/PythonScript.h steps/PythonScriptLooped.h)
else(CEDAR_USE_PYTHONSTEP)
  set(pythonstep_moc_headers "")
endif(CEDAR_USE_PYTHONSTEP)


cedar_add_library(cedarproc
  LINK_LIBRARIES
  cedaraux
  cedarunits
  cedardev
  MOC_HEADERS
  auxiliaries/gui/AccelerationParameter.h
  auxiliaries/gui/AngularVelocityParameter.h
  auxiliaries/gui/BoolParameter.h
  auxiliaries/gui/BoolVectorParameter.h
  auxiliaries/gui/Configurable.h
  auxiliaries/gui/DataPlotter.h
  auxiliaries/gui/DirectoryParameter.h
  auxiliaries/gui/DoubleParameter.h
  auxiliaries/gui/DoubleVectorParameter.h
  auxiliaries/gui/EnumParameter.h
  auxiliaries/gui/FileParameter.h
  auxiliaries/gui/FrequencyParameter.h
  auxiliaries/gui/IgnoreLocaleDoubleSpinBox.h
  auxiliaries/gui/IntParameter.h
  auxiliaries/gui/IntVectorParameter.h
  auxiliaries/gui/LengthParameter.h
  auxiliaries/gui/ObjectParameter.h
  auxiliaries/gui/ObjectListParameter.h
  auxiliaries/gui/Parameter.h
  auxiliaries/gui/PlaneAngleParameter.h
  auxiliaries/gui/PropertyPane.h
  auxiliaries/gui/SettingsWidget.h
  auxiliaries/gui/StringParameter.h
  auxiliaries/gui/StringVectorParameter.h
  auxiliaries/gui/TimeParameter.h
  auxiliaries/gui/UIntParameter.h
  auxiliaries/gui/UIntVectorParameter.h
  auxiliaries/gui/VelocityParameter.h
  devices/gui/ComponentParameter.h
  devices/gui/RobotManager.h
  experiment/Experiment.h
  experiment/ActionParameter.h
  experiment/PropertyParameter.h
  experiment/ConnectableParameter.h
  experiment/StepPropertyParameter.h
  experiment/Supervisor.h
  experiment/condition/OnMatrixValue.h
  experiment/action/IterateDirectoryContents.h
  experiment/gui/ActionListParameter.h
  experiment/gui/ActionSequence.h
  experiment/gui/ActionParameter.h
  experiment/gui/ExperimentItemWidget.h
  experiment/gui/ConnectableParameter.h
  experiment/gui/StepPropertyParameter.h
  gui/ArchitectureWidgetList.h
  gui/ArchitectureScriptEditor.h
  gui/stepViews/BoostView.h
  gui/stepViews/StaticGainView.h
  gui/stepViews/ComponentStepView.h
  gui/stepViews/LinearSpatialTemplateView.h
  gui/stepViews/NaNCheckView.h
  gui/stepViews/LinearFunctionView.h
  gui/stepViews/ConstVectorView.h
  gui/stepViews/SinusFunctionView.h
  gui/stepViews/SinusSignalView.h
  gui/AdvancedParameterLinker.h
  gui/ArchitectureConsistencyCheck.h
  gui/ArchitectureWidget.h
  gui/AutoPositionStepsDialog.h
  gui/BoostControl.h
  ${copyscript_moc_headers}
  gui/CodeWidget.h
  gui/CommentWidget.h
  gui/Connectable.h
  gui/ConnectionAnchor.h
  gui/CouplingCollection.h
  gui/DataSlotItem.h
  gui/ElementList.h
  gui/ElementTreeWidget.h
  gui/ExperimentDialog.h
  gui/FindDialog.h
  gui/GroupContainerItem.h
  gui/GroupParameterDesigner.h
  gui/Ide.h
  gui/IdeApplication.h
  gui/OneTimeMessageDialog.h
  gui/PlotWidget.h
  gui/PlotDockWidget.h
  gui/ProjectionMappingParameter.h
  gui/PerformanceOverview.h
  gui/RecorderWidget.h
  gui/RecorderProperty.h
  gui/Scene.h
  gui/Settings.h
  gui/SettingsDialog.h
  gui/SimulationControl.h
  gui/StepItem.h
  gui/StepManager.h
  gui/TriggerItem.h
  gui/Group.h
  gui/ToolBox.h
  gui/View.h
  gui/WidgetContainerItem.h
  sources/LocalReader.h
  sinks/LocalWriter.h
  sinks/TCPWriter.h
  sources/TCPReader.h
  sources/BoxInput.h
  sources/GaussInput.h
  sources/RealSense.h
  ${pythonstep_moc_headers}
  steps/VariableBox.h
  steps/VariableGauss.h
  steps/Sinus.h
  steps/LinearFunction.h
  steps/Logarithm.h
  steps/AllocentricToDistanceImage.h
  steps/BufferReader.h
  steps/Component.h
  steps/Convolution.h
  steps/NeuralCoordinateTransform.h
  steps/ConvertToValueRange.h
  steps/ColorConversion.h
  steps/ComponentMultiply.h
  steps/CoordinateToCameraAngles.h
  steps/DivideElementwise.h
  steps/DNN.h
  steps/SubtractElementwise.h
  steps/CoordinateTransformation.h
  steps/ForwardKinematics.h
  steps/PseudoInverseKinematics.h
  steps/DampedPseudoInverseKinematics.h
  steps/DistanceImageCoordinateTransform.h
  steps/ConfigurationLimits.h
  steps/PseudoInverse.h
  steps/VehicleObstacleAvoidanceForceLet.h
  steps/Jacobian.h
  steps/MatrixMultiply.h
  steps/MatrixPadding.h
  steps/MatrixSlice.h
  steps/MatrixTypeConverter.h
  steps/Normalization.h
  steps/Flip.h
  steps/AnglesOnPlane.h
  steps/StaticGain.h
  steps/ScalarsToVector.h
  steps/ConstVector.h
  steps/Stack.h
  steps/pairsGame.h
  steps/ShapeVisualisation.h
  steps/ShiftedMultiplication.h
  steps/UnitConverter.h
  steps/Histogram.h
  steps/ScalarDivision.h
  steps/TemplateMatching.h
  steps/Threshold.h
  steps/Clamp.h
  steps/TransferFunction.h
  steps/VectorToScalars.h
  steps/VehicleRotation.h
  steps/Delay.h
  steps/WeightedSum.h
  steps/PythonScript.h
  steps/PythonScriptLooped.h
  ${qglviewer_moc_headers}
  sources/Boost.h
  sources/Noise.h
  sources/Picture.h
  sources/Video.h
  sources/GrabberBase.h
  sources/Camera.h
  sources/CameraStream.h
  sources/SpatialTemplate.h
  sources/LinearSpatialTemplate.h
  sources/ConstDiagonalMatrix.h
  sources/ConstMatrix.h
  steps/Projection.h
  steps/Resize.h
  steps/ImageTransform.h
  steps/AddConstant.h
  steps/Count.h
  steps/Switch.h
  steps/SynapticConnection.h
  steps/LinearLateralShift.h
  steps/HopfOscillatorRHS.h
  steps/HarmonicOscillatorRHS.h
  steps/NaNCheck.h
  steps/PositionOfMaximum.h
  steps/DiagonalReadout.h
  steps/HeadCenteredTransform.h
  steps/InvHeadCenteredTransform.h
  CppScript.h
  Group.h
  Step.h
  LoopedTrigger.h
  ${libdc_moc_headers}
  ${yarp_moc_headers}
  RESOURCES
  gui/icons/cedar_proc_gui_icons.qrc
  gui/images/cedar_proc_gui_images.qrc
  )
