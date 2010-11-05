cmake_minimum_required(VERSION 2.8)

# set the cedar home directory to the current directory
set(CEDAR_HOME ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Home of cedar; should NOT be altered.")

# include config file
set(CEDAR_CONF_FILE ${CEDAR_HOME}/cedar.conf CACHE FILEPATH "Default configuration file of cedar.")
message (STATUS "including ${CEDAR_CONF_FILE} configuration file")
include(${CEDAR_CONF_FILE})

set (CEDAR_SOURCE_DIR     "${CEDAR_HOME}/src"         CACHE PATH "Directory all library source code will be stored in.")
set (CEDAR_CMAKE_DIR      "${CEDAR_HOME}/cmake"       CACHE PATH "Directory all additional CMake files will be stored in.")
set (CEDAR_UNIT_TEST_DIR  "${CEDAR_HOME}/tests/unit"  CACHE PATH "Directory all unit tests will be stored in.")

# include the macro section
include(${CEDAR_CMAKE_DIR}/macros.txt)

# go on with the setup of cedar
include(${CEDAR_CMAKE_DIR}/setup.txt)


