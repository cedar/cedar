# Find FFTW and create the following output:
# FFTW_FOUND - whether FFTW was found or not
# FFTW_INCLUDE_DIRS - the FFTW include directories
# FFTW_LIBS - FFTW libraries

# find include dir in set of paths
find_path(FFTW_INCLUDE_DIRS
  NAMES fftw3.h
  PATHS ${CEDAR_DEPENDENCY_FFTW}
)
# find library in set of paths
find_library(FFTW_LIBS
  NAMES fftw3 fftw3_threads
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)
# now check if anything is missing
if(FFTW_INCLUDE_DIRS AND FFTW_LIBS)
  set(FFTW_FOUND true)
else(FFTW_INCLUDE_DIRS AND FFTW_LIBS)
  set(FFTW_FOUND false)
endif(FFTW_INCLUDE_DIRS AND FFTW_LIBS)

