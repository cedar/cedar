# Find QT57 and create the following output:
# QT_FOUND - whether QT57 was found or not
# Qt5_!PACKAGENAME!_INCLUDE_DIRS - the include directories for the different Qt5 Packages
# QT_VERSION - will be set to 5
# QT_UIC_EXECUTABLE - will be set to the 5.7 executable

message("-- looking for Qt5...")
# Set the Qt5 Path to the CMakePrefix Path according to http://doc.qt.io/qt-5/cmake-manual.html
set(CMAKE_PREFIX_PATH ${CEDAR_DEPENDENCY_QT5}/lib/cmake)
find_package(Qt5Widgets 5.7.0 REQUIRED)
if(Qt5Widgets_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5Widgets_LIBRARIES})
  include_directories(${Qt5Widgets_INCLUDE_DIRS})
  add_definitions(${Qt5Widgets_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5Widgets was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5Widgets_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5Widgets was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5Widgets_FOUND)

find_package(Qt5Charts 5.7.0 REQUIRED)
if(Qt5Charts_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5Charts_LIBRARIES})
  include_directories(${Qt5Charts_INCLUDE_DIRS})
  add_definitions(${Qt5Charts_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5Charts was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5Charts_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5Charts was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5Charts_FOUND)

find_package(Qt5DataVisualization 5.7.0 REQUIRED)
if(Qt5DataVisualization_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5DataVisualization_LIBRARIES})
  include_directories(${Qt5DataVisualization_INCLUDE_DIRS})
  add_definitions(${Qt5DataVisualization_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5DataVisualization was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5DataVisualization_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5DataVisualization was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5DataVisualization_FOUND)

find_package(Qt53DCore 5.7.0 REQUIRED)
if(Qt53DCore_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt53DCore_LIBRARIES})
  include_directories(${Qt53DCore_INCLUDE_DIRS})
  add_definitions(${Qt53DCore_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt53DCore was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt53DCore_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt53DCore was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt53DCore_FOUND)

find_package(Qt53DExtras 5.7.0 REQUIRED)
if(Qt53DExtras_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt53DExtras_LIBRARIES})
  include_directories(${Qt53DExtras_INCLUDE_DIRS})
  add_definitions(${Qt53DExtras_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt53DExtras was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt53DExtras_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt53DExtras was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt53DExtras_FOUND)

find_package(Qt53DInput 5.7.0 REQUIRED)
if(Qt53DInput_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt53DInput_LIBRARIES})
  include_directories(${Qt53DInput_INCLUDE_DIRS})
  add_definitions(${Qt53DInput_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt53DInput was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt53DInput_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt53DInput was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt53DInput_FOUND)

find_package(Qt53DLogic 5.7.0 REQUIRED)
if(Qt53DLogic_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt53DLogic_LIBRARIES})
  include_directories(${Qt53DLogic_INCLUDE_DIRS})
  add_definitions(${Qt53DLogic_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt53DLogic was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt53DLogic_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt53DLogic was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt53DLogic_FOUND)

find_package(Qt53DRender 5.7.0 REQUIRED)
if(Qt53DRender_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt53DRender_LIBRARIES})
  include_directories(${Qt53DRender_INCLUDE_DIRS})
  add_definitions(${Qt53DRender_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt53DRender was found in ${Qt5Widgets_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt53DRender_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt53DRender was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt53DRender_FOUND)

find_package(Qt5OpenGL 5.7.0 REQUIRED)
if(Qt5OpenGL_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5OpenGL_LIBRARIES})
  include_directories(${Qt5OpenGL_INCLUDE_DIRS})
  add_definitions(${Qt5OpenGL_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5OpenGL was found in ${Qt5OpenGL_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5OpenGL_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5OpenGL was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5OpenGL_FOUND)

find_package(Qt5Xml 5.7.0 REQUIRED)
if(Qt5Xml_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5Xml_LIBRARIES})
  include_directories(${Qt5Xml_INCLUDE_DIRS})
  add_definitions(${Qt5Xml_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5Xml was found in ${Qt5Xml_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5Xml_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5Xml was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5Xml_FOUND)

find_package(Qt5Svg 5.7.0 REQUIRED)
if(Qt5Svg_FOUND)
  set(CEDAR_THIRD_PARTY_LIBS ${CEDAR_THIRD_PARTY_LIBS} ${Qt5Svg_LIBRARIES})
  include_directories(${Qt5Svg_INCLUDE_DIRS})
  add_definitions(${Qt5Svg_DEFINITIONS})
  if (CEDAR_VERBOSE_BUILD_SYSTEM)
    message("   Qt5Svg was found in ${Qt5Svg_INCLUDE_DIRS}")
  endif(CEDAR_VERBOSE_BUILD_SYSTEM)
else(Qt5Svg_FOUND)
  message(${CEDAR_LIB_ERROR_TYPE} "   Required library Qt5Svg was not found on your system!
    You can try to alter the default search paths in cedar.conf.")
endif(Qt5Svg_FOUND)

if(${Qt5Widgets_FOUND} AND ${Qt5Svg_FOUND} AND ${Qt5Xml_FOUND} AND ${Qt5OpenGL_FOUND} AND ${Qt5Charts_FOUND} AND ${Qt5DataVisualization_FOUND} AND ${Qt53DCore_FOUND} AND ${Qt53DExtras_FOUND} AND ${Qt53DInput_FOUND} AND ${Qt53DLogic_FOUND} AND ${Qt53DRender_FOUND})
  SET(QT_FOUND 1)
  SET(QT_VERSION 5)
  message("-- QT5 was found")
  message("-- Thus QWT and QWTPlot3D Plotting libraries might be disabled!")
else()
  SET(QT_FOUND 0)
endif()
SET(QT5_FOUND ${QT_FOUND})
set(QT_UIC_EXECUTABLE ${CEDAR_DEPENDENCY_QT5}/bin/uic)




