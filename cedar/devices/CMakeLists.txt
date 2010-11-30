cedar_add_library(cedardev
  robot/KinematicChain.cpp
  robot/KinematicChainModel.cpp
#  robot/KinematicChainVisualization.cpp
  robot/KinematicChainSimulation.cpp
  robot/Component.cpp
  robot/ReferenceGeometry.cpp
  robot/Robot.cpp
  robot/ComponentNotAvailableException.cpp
#  robot/CoraSimulation.cpp
  HEADERS
  robot/KinematicChain.h
  robot/KinematicChainModel.h
#  robot/KinematicChainVisualization.h
  robot/KinematicChainSimulation.h
  robot/Component.h
  robot/ReferenceGeometry.h
  robot/Robot.h
  robot/ComponentNotAvailableException.h
#  robot/CoraSimulation.h
  LINK_LIBRARIES
  cedaraux
)
