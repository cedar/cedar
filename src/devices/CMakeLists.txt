cedar_add_library(devices
  robot/KinematicChain.h
  robot/KinematicChain.cpp
  robot/KinematicChainModel.h
  robot/Component.h
  robot/Component.cpp
  robot/ReferenceGeometry.h
  robot/ReferenceGeometry.cpp
  robot/Robot.h
  robot/Robot.cpp
  robot/ComponentNotAvailableException.h
  robot/ComponentNotAvailableException.cpp
  LINK_LIBRARIES auxiliaries
)
