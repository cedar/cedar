# Find Libconfig and create the following output:
# Libconfig_FOUND - whether Libconfig was found or not
# Libconfig_INCLUDE_DIRS - the Libconfig include directories
# Libconfig_LIBS - Libconfig libraries

# find include dir in set of paths
find_path(Libconfig_INCLUDE_DIRS
  NAMES libconfig.h libconfig.h++
  PATHS ${CEDAR_DEPENDENCY_LIBCONFIG}
)
# find library in set of paths
find_library(Libconfig_LIBS
  NAMES config++
  PATHS ${CEDAR_DEPENDENCY_LIBRARIES}
)
# now check if anything is missing
if(Libconfig_INCLUDE_DIRS AND Libconfig_LIBS)
  set(Libconfig_FOUND true)
else(Libconfig_INCLUDE_DIRS AND Libconfig_LIBS)
  set(Libconfig_FOUND false)
endif(Libconfig_INCLUDE_DIRS AND Libconfig_LIBS)


