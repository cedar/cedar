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
# find library in set of paths
if(NOT CEDAR_USE_QT5)
    find_library(QGLViewer_LIBS
            NAMES libPythonQt-Qt5-Python3.6 libPythonQt_QtAll-Qt5-Python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}
            PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
            )
else(NOT CEDAR_USE_QT5)
    if(APPLE)
        find_library(QGLViewer_LIBS
                NAMES qglviewer-qt5 QGLViewer-qt5 QGLViewer qglviewer
                PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
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
                    NAMES PythonQt_QtAll-Qt5-Python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.lib
                    PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                    )
			find_library(PYTHONQT_LIBS_TMP
                    NAMES PythonQt-Qt5-Python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.lib
                    PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${PYTHONQT_PATH_LIB}
                    )
        endif(NOT WIN32)		
    endif(APPLE)
	set(PYTHONQT_LIBS ${PYTHONQT_LIBS} ${PYTHONQT_LIBS_TMP})
endif(NOT CEDAR_USE_QT5)

# now check if anything is missing
if(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
    set(QGLViewer_FOUND true)
else(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
    set(QGLViewer_FOUND false)
endif(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)