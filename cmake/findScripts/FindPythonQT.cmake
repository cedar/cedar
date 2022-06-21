# Find QGLViewer and create the following output:
# QGLViewer_FOUND - whether QGLViewer was found or not
# QGLViewer_INCLUDE_DIRS - the QGLViewer include directories
# QGLViewer_LIBS - QGLViewer libraries

# find include dir in set of paths
set(PYTHONQT_PATH_LIB ${PYTHONQT_PATH}/lib)
set(PYTHONQT_PATH_INCLUDE ${PYTHONQT_PATH}/src)

find_path(PYTHONQT_INCLUDE_DIRS
        NAMES PythonQt.h
        PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_INCLUDE}
        )

if(APPLE)
    find_library(PYTHONQT_LIBS
            NAMES libPythonQt_QtAll-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.dylib
            PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
            )
    find_library(PYTHONQT_LIBS_TMP
            NAMES libPythonQt-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.dylib
            PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
            )
else(APPLE)
    if(NOT WIN32)
        find_library(PYTHONQT_LIBS
                NAMES libPythonQt_QtAll-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.so
                PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                )
        find_library(PYTHONQT_LIBS_TMP
                NAMES libPythonQt-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.so
                PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                )

    else(NOT WIN32)
        find_library(PYTHONQT_LIBS
                NAMES libPythonQt_QtAll-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.dll
                PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                )
        find_library(PYTHONQT_LIBS_TMP
                NAMES libPythonQt-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.dll
                PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                )
    endif(NOT WIN32)
endif(APPLE)

set(PYTHONQT_LIBS ${PYTHONQT_LIBS} ${PYTHONQT_LIBS_TMP})
