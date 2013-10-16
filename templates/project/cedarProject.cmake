#=======================================================================================================================
#
#   Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
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
#   Maintainer:  Oliver Lomp
#   Email:       oliver.lomp@ini.ruhr-uni-bochum.de
#   Date:        2012 06 27
#
#   Description: This is a script for building projects that use cedar. Include it in your CMakeLists.txt and use the
#                macros provided below for easy building of shared objects and executables.
#
#   Credits:
#
#=======================================================================================================================

# Set some default configuration values
set(CEDAR_BUILD_DIR build)
set(CEDAR_LIB_DIR lib)
set(Boost_Added Off)

# Read the user's
file(GLOB conf_check "project.conf")

if (NOT conf_check)
  file(GLOB conf_example_check "project.conf.example")
  if (conf_example_check)
    message(WARNING "Configuration file not found. Using example file, please check correctness of the paths!")
    configure_file("project.conf.example" "${CMAKE_CURRENT_SOURCE_DIR}/project.conf" COPYONLY)
  else()
    message(FATAL_ERROR "Neither the project.conf nor the project.conf.example file could be found. Please provide configuration files!")
  endif()
endif()

include("project.conf")

if (DEBUG_CEDAR_BUILD_SYSTEM)
  message ("  >> cedar home is set to ${CEDAR_HOME}")
endif(DEBUG_CEDAR_BUILD_SYSTEM)

if(CEDAR_HOME)
  message("-- Using local cedar version in ${CEDAR_HOME}")
  # Add include directories
  include_directories("${CEDAR_HOME}" "${CEDAR_HOME}/${CEDAR_BUILD_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

  # Add link directories
  link_directories("${CEDAR_HOME}/${CEDAR_LIB_DIR}")
  
  # includes and libraries of external dependencies
  include("${CEDAR_HOME}/${CEDAR_BUILD_DIR}/cedar_configuration.cmake")
  
elseif(CEDAR_HOME_INSTALLED)
  message("-- Using installed cedar version in ${CEDAR_HOME_INSTALLED}")
  # Add include directories
  include_directories("${CEDAR_HOME_INSTALLED}/include" "${CMAKE_CURRENT_BINARY_DIR}")

  # Add link directories
  link_directories("${CEDAR_HOME_INSTALLED}/lib")

  # includes and libraries of external dependencies
  include("${CEDAR_HOME_INSTALLED}/share/cedar/cedar_configuration.cmake")
endif(CEDAR_HOME)

## Macros ########################

macro(cedar_project_depends_on target DEPENDS_ON other_target)
  add_dependencies(${target} ${other_target})
  
  get_target_property(type ${other_target} TYPE)
  
  if (${type} MATCHES "SHARED_LIBRARY")
    if(DEBUG_CEDAR_BUILD_SYSTEM)
      message("  >> Linking libraries from target ${other_target} into ${target}")
    endif(DEBUG_CEDAR_BUILD_SYSTEM)
    
    target_link_libraries(${target} ${other_target})
  endif()
endmacro(cedar_project_depends_on)


#
# Add a new target to be built by cedar's project system.
#
# Format: cedar_project_add_target({LIBRARY|EXECUTABLE} path
#                                  [TARGET_NAME target]
#                                  [MOC_HEADERS moc1 moc2 ...]
#                                  [DEPENDS_ON target1 target2 ...])
#
macro(cedar_project_add_target)
  cedar_project_parse_arguments(add "LIBRARY;EXECUTABLE;TARGET_NAME;MOC_HEADERS;DEPENDS_ON" "" ${ARGN})
  
  if (add_EXECUTABLE)
    set(directory ${add_EXECUTABLE})
    set(target_type "executable")
  endif(add_EXECUTABLE)
  
  if (add_LIBRARY)
    set(directory ${add_LIBRARY})
    set(target_type "library")
  endif(add_LIBRARY)
  
  if (add_TARGET_NAME)
    set(target_name ${add_TARGET_NAME})
  else (add_TARGET_NAME)
    set(target_name ${directory})
  endif (add_TARGET_NAME)
  
  set(moc_headers "")
  foreach (moc_header ${add_MOC_HEADERS})
    list(APPEND moc_headers ${directory}/${moc_header})
  endforeach()
  
  message("-- Adding ${target_type} ${target_name} in ${directory}")
    
  cedar_project_setup(${directory})
  
  # List all files in the directory that need to be compiled
  list_files_to_compile(${directory})
  
  # Set moc files to be compiled
  
  # Set the output directory to something that prevents naming conflicts
  set(old_binary_dir ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_project_files)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  
  qt_add_resources(compiled_resource_paths ${project_resources})
  qt_wrap_cpp(moc_headers ${moc_headers})
  qt_wrap_ui(forms ${project_forms})
  
  set(CMAKE_CURRENT_BINARY_DIR ${old_binary_dir})
  
  set(files ${project_sources} ${moc_headers} ${forms} ${compiled_resource_paths})
  
  if (target_type MATCHES "executable")
    add_executable(${target_name} ${files})
  else()
    add_library(${target_name} SHARED ${files})
  endif()
  
  target_link_libraries(${target_name} cedarunits cedaraux cedardev cedarproc cedardyn ${CEDAR_EXTERNAL_LIBS})
  
  foreach (dependency ${add_DEPENDS_ON})
    cedar_project_depends_on(${target_name} DEPENDS_ON ${dependency})
  endforeach()
  
endmacro(cedar_project_add_target)


  
macro(cedar_project_setup directory)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${directory}")
  
  # Add some platform-specific definitions  
  if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Wextra)
    add_definitions(-DGNUCC -D__GNUCC__ -D__GCC__ -DGCC)
    add_definitions(-std=gnu++0x)
  elseif (MSVC)
    add_definitions(-W2)
    add_definitions(-DMSVC)
  endif (CMAKE_COMPILER_IS_GNUCC)
  
endmacro(cedar_project_setup)



macro(list_files_to_compile directory)
  file(GLOB_RECURSE project_headers ${directory}/*.h)
  file(GLOB_RECURSE project_forms ${directory}/*.ui)
  file(GLOB_RECURSE project_resources ${directory}/*.qrc)
  file(GLOB_RECURSE project_sources ${directory}/*.cpp)
  file(GLOB_RECURSE project_sources_c ${directory}/*.c)
  list(APPEND project_sources ${project_sources_c})
endmacro(list_files_to_compile)



macro(cedar_project_parse_arguments prefix arg_names option_names)
  set(DEFAULT_ARGS)
  foreach(arg_name ${arg_names})    
    set(${prefix}_${arg_name} "")
  endforeach(arg_name)
  
  foreach(option ${option_names})
    set(${prefix}_${option} FALSE)
  endforeach(option)

  set(current_arg_name DEFAULT_ARGS)
  set(current_arg_list)
  foreach(arg ${ARGN})            
    set(larg_names ${arg_names})    
    list(FIND larg_names "${arg}" is_arg_name)                   
    if (is_arg_name GREATER -1)
      set(${prefix}_${current_arg_name} ${current_arg_list})
      set(current_arg_name ${arg})
      set(current_arg_list)
    else (is_arg_name GREATER -1)
      set(loption_names ${option_names})    
      LIST(FIND loption_names "${arg}" is_option)            
      if (is_option GREATER -1)
        set(${prefix}_${arg} TRUE)
      else (is_option GREATER -1)
        set(current_arg_list ${current_arg_list} ${arg})
      endif (is_option GREATER -1)
    endif (is_arg_name GREATER -1)
  endforeach(arg)
  set(${prefix}_${current_arg_name} ${current_arg_list})
endmacro(cedar_project_parse_arguments)

################################################################################################################
# Wrapper around QT4/5 macros
#

macro(qt_wrap_cpp)
  if(QT_VERSION GREATER 4)
    qt5_wrap_cpp(${ARGV})
  else(QT_VERSION GREATER 4)
    qt4_wrap_cpp(${ARGV})
  endif(QT_VERSION GREATER 4)
endmacro(qt_wrap_cpp)

macro(qt_add_resources)
  if(QT_VERSION GREATER 4)
    qt5_add_resources(${ARGV})
  else(QT_VERSION GREATER 4)
    qt4_add_resources(${ARGV})
  endif(QT_VERSION GREATER 4)
endmacro(qt_add_resources)

macro(qt_wrap_ui ${ARGV})
  if(QT_VERSION GREATER 4)
    qt5_wrap_ui(${ARGV})
  else(QT_VERSION GREATER 4)
    qt4_wrap_ui(${ARGV})
  endif(QT_VERSION GREATER 4)
endmacro(qt_wrap_ui)