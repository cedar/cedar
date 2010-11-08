add_library(devices ${CEDAR_LIB_TYPE} 
  robot/KinematicChain.h
  robot/KinematicChainModel.h
  robot/Component.h
  robot/Component.cpp
  robot/ReferenceGeometry.h
  robot/ReferenceGeometry.cpp
  robot/Robot.h
  robot/Robot.cpp
  robot/ComponentNotAvailableException.h
  robot/ComponentNotAvailableException.cpp
)
