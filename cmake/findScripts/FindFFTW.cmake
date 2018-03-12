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
  NAMES fftw3 libfftw3 libfftw3-3
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)
find_library(FFTW_LIBS_THREADED
#  NAMES fftw3_threads
  NAMES fftw3_omp
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)

# now check if anything is missing
if(FFTW_INCLUDE_DIRS AND FFTW_LIBS)
  set(FFTW_FOUND true)
else(FFTW_INCLUDE_DIRS AND FFTW_LIBS)
  set(FFTW_INCLUDE_DIRS "")
  set(FFTW_LIBS "")
  set(FFTW_FOUND false)
endif(FFTW_INCLUDE_DIRS AND FFTW_LIBS)

if(FFTW_LIBS_THREADED)
  set(FFTW_THREADED true)
else(FFTW_LIBS_THREADED)
  set(FFTW_LIBS_THREADED "")
    set(FFTW_THREADED false)
endif(FFTW_LIBS_THREADED)

