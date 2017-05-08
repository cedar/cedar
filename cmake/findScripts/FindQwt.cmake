# Find qwt and create the following output:
# qwt_FOUND - whether qwt was found or not
# qwt_INCLUDE_DIRS - the qwt include directories
# qwt_LIBS - qwt libraries

# find include dir in set of paths
find_path(qwt_INCLUDE_DIRS
  NAMES qwt.h
  PATHS ${CEDAR_DEPENDENCY_QWT}
)
# find library in set of paths
if(NOT CEDAR_USE_QT5)
  find_library(qwt_LIBS
    NAMES qwt-qt4 qwt
    PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
  )
else(NOT CEDAR_USE_QT5)
  find_library(qwt_LIBS
    NAMES qwt-qt5
    PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
  )
endif(NOT CEDAR_USE_QT5)
# now check if anything is missing
if(qwt_INCLUDE_DIRS AND qwt_LIBS)
  set(qwt_FOUND true)
else(qwt_INCLUDE_DIRS AND qwt_LIBS)
  set(qwt_FOUND false)
  set(qwt_LIBS "")
  set(qwt_INCLUDE_DIRS "")
endif(qwt_INCLUDE_DIRS AND qwt_LIBS)

