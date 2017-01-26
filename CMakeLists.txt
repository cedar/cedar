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

cmake_minimum_required(VERSION 2.8)

# set the cedar home directory to the current directory
set(CEDAR_HOME ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Home of cedar; should NOT be altered.")

# include config file
set(CEDAR_CONF_FILE ${CEDAR_HOME}/cedar.conf CACHE FILEPATH "Default configuration file of cedar.")

# check if the config file exists 
if (NOT EXISTS ${CEDAR_CONF_FILE})
  message(FATAL_ERROR "The cedar.conf file could not be found. Please create it, e.g., by copying cedar.conf.example to ${CEDAR_CONF_FILE} and filling out the proper values.")
endif(NOT EXISTS ${CEDAR_CONF_FILE})

message (STATUS "including ${CEDAR_CONF_FILE} configuration file")
include(${CEDAR_CONF_FILE})

set(CEDAR_CMAKE_DIR "${CEDAR_HOME}/cmake" CACHE PATH "Directory all additional CMake files will be stored in.")

# make a sanity check of what we have read from the configuration file thus far
include(${CEDAR_CMAKE_DIR}/sanity.txt)

# include the macro section
include(${CEDAR_CMAKE_DIR}/macros.txt)

#include the version information
include(${CEDAR_CMAKE_DIR}/version.txt)

# general setup of cedar variables
include(${CEDAR_CMAKE_DIR}/setup.txt)

# setup of all cedar and third party libraries
include(${CEDAR_CMAKE_DIR}/libraries.txt)

# setup of unit and interactive tests
include(${CEDAR_CMAKE_DIR}/testing.txt)

# setup of executables
include(${CEDAR_CMAKE_DIR}/executables.txt)

# setup documentation
include(${CEDAR_CMAKE_DIR}/doxygen.txt)

# setup of files that are installed (additionally to the libraries)
include(${CEDAR_CMAKE_DIR}/install.txt)

# setup of packaging
include(${CEDAR_CMAKE_DIR}/packaging.txt)

# output configuration summary
include(${CEDAR_CMAKE_DIR}/summary.txt)

