# Find GLEW and create the following output:
# GLEW_FOUND - whether GLEW was found or not
# GLEW_INCLUDE_DIRS - the GLEW include directories
# GLEW_LIBS - GLEW libraries

# find include dir in set of paths
find_path(GLEW_INCLUDE_DIRS
  NAMES glew.h
  PATHS ${CEDAR_DEPENDENCY_GLEW} ${CEDAR_DEPENDENCY_LIBRARIES}
)
# find library in set of paths
find_library(GLEW_LIBS
  NAMES GLEW glew32 glew glew32s 
  PATH_SUFFIXES lib64
  PATHS ${CEDAR_DEPENDENCY_GLEW} ${CEDAR_DEPENDENCY_LIBRARIES}
)

# now check if anything is missing
if(GLEW_INCLUDE_DIRS AND GLEW_LIBS)
  set(GLEW_FOUND true)
else(GLEW_INCLUDE_DIRS AND GLEW_LIBS)
  set(GLEW_INCLUDE_DIRS "")
  set(GLEW_LIBS "")
  set(GLEW_FOUND false)
endif(GLEW_INCLUDE_DIRS AND GLEW_LIBS)

