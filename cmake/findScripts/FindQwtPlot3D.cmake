# Find qwtplot3d and create the following output:
# qwtplot3d_FOUND - whether qwtplot3d was found or not
# qwtplot3d_INCLUDE_DIRS - the qwtplot3d include directories
# qwtplot3d_LIBS - qwtplot3d libraries

# find include dir in set of paths
find_path(qwtplot3d_INCLUDE_DIRS
  NAMES qwt3d_plot3d.h qwt3d_gridplot.h
  PATHS ${CEDAR_DEPENDENCY_QWTPLOT3D}
)
# find library in set of paths
find_library(qwtplot3d_LIBS
  NAMES qwtplot3d
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)
# now check if anything is missing
if(qwtplot3d_INCLUDE_DIRS AND qwtplot3d_LIBS)
  set(qwtplot3d_FOUND true)
else(qwtplot3d_INCLUDE_DIRS AND qwtplot3d_LIBS)
  set(qwtplot3d_FOUND false)
  set(qwtplot3d_LIBS "")
  set(qwtplot3d_INCLUDE_DIRS "")
endif(qwtplot3d_INCLUDE_DIRS AND qwtplot3d_LIBS)
