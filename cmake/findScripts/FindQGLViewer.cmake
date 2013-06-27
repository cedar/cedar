# Find QGLViewer and create the following output:
# QGLViewer_FOUND - whether QGLViewer was found or not
# QGLViewer_INCLUDE_DIRS - the QGLViewer include directories
# QGLViewer_LIBS - QGLViewer libraries

# find include dir in set of paths
find_path(QGLViewer_INCLUDE_DIRS
  NAMES qglviewer.h
  PATHS ${CEDAR_DEPENDENCY_QGLVIEWER}
)
# find library in set of paths
find_library(QGLViewer_LIBS
  NAMES qglviewer-qt4 QGLViewer qglviewer
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)

# now check if anything is missing
if(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
  set(QGLViewer_FOUND true)
else(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)
  set(QGLViewer_FOUND false)
endif(QGLViewer_INCLUDE_DIRS AND QGLViewer_LIBS)

