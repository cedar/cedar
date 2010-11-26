cmake_minimum_required(VERSION 2.8)

# set the cedar home directory to the current directory
set(CEDAR_HOME ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Home of cedar; should NOT be altered.")

# include config file
set(CEDAR_CONF_FILE ${CEDAR_HOME}/cedar.conf CACHE FILEPATH "Default configuration file of cedar.")
message (STATUS "including ${CEDAR_CONF_FILE} configuration file")
include(${CEDAR_CONF_FILE})

set(CEDAR_CMAKE_DIR "${CEDAR_HOME}/cmake" CACHE PATH "Directory all additional CMake files will be stored in.")

# include the macro section
include(${CEDAR_CMAKE_DIR}/macros.txt)

# general setup of cedar variables
include(${CEDAR_CMAKE_DIR}/setup.txt)

# setup of all cedar and third party libraries
include(${CEDAR_CMAKE_DIR}/libraries.txt)

# setup of unit and interactive tests
include(${CEDAR_CMAKE_DIR}/testing.txt)

# setup of files that are installed (additionally to the libraries)
include(${CEDAR_CMAKE_DIR}/install.txt)

# setup of packaging
include(${CEDAR_CMAKE_DIR}/packaging.txt)


