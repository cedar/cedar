# Find QGLViewer and create the following output:
# QGLViewer_FOUND - whether QGLViewer was found or not
# QGLViewer_INCLUDE_DIRS - the QGLViewer include directories
# QGLViewer_LIBS - QGLViewer libraries

# find include dir in set of paths
find_path(QGLViewer_INCLUDE_DIRS
  NAMES qglviewer.h
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
)
# find library in set of paths
if(NOT CEDAR_USE_QT5)
  find_library(QGLViewer_LIBS
    NAMES qglviewer-qt4 QGLViewer-qt4 QGLViewer qglviewer
    PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
  )
else(NOT CEDAR_USE_QT5)
  if(APPLE)
    find_library(QGLViewer_LIBS
            NAMES qglviewer-qt5 QGLViewer-qt5 QGLViewer qglviewer
            PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
            )
  else(APPLE)
    if(NOT WIN32)
    find_library(QGLViewer_LIBS
            NAMES qglviewer-qt5 QGLViewer-qt5 #QGLViewer qglviewer
           	PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
            )
    else(NOT WIN32)
    find_library(QGLViewer_LIBS
            NAMES qglviewer-qt5 QGLViewer-qt5 QGLViewer qglviewer QGLViewer2 QGLViewer2.dll
            PATHS ${CEDAR_DEPENDENCY_LIBRARIES} ${CEDAR_DEPENDENCY_QGLVIEWER}
            )
    endif(NOT WIN32)
  endif(APPLE)
endif(NOT CEDAR_USE_QT5)

# now check if anything is missing
if(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
  set(QGLViewer_FOUND true)
else(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
  set(QGLViewer_FOUND false)
endif(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)

