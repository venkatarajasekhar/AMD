# Generate header file
include (CheckIncludeFileCXX)

check_include_file_cxx ("time.h" AMD_HAVE_TIME_H CACHE BOOLEAN ON)
check_include_file_cxx ("sys/time.h" AMD_HAVE_SYS_TIME_H CACHE BOOLEAN ON)
check_include_file_cxx ("ctime" AMD_HAVE_CTIME CACHE BOOLEAN ON)

# Get the operating system that is in use
if (CMAKE_SYSTEM MATCHES "Linux")
  set (AMD_LINUX 1)
#elseif (CMAKE_SYSTEM MATCHES "Windows")
#  message (STATUS "We only support 32-bit Windows as of now")
#  set (AMD_WINDOWS 1)
#elseif (CMAKE_SYSTEM MATCHES "Darwin")
#  set (AMD_DARWIN 1)
#elseif (CMAKE_SYSTEM MATCHES "AIX")
#  set (AMD_AIX 1)
#else (CMAKE_SYSTEM MATCHES "Linux")
#  message (FATAL_ERROR "PFunc is currently only supported on Linux, Windows,
#  AIX and Darwin")
else (CMAKE_SYSTEM MATCHES "Linux")
  set (AMD_WINDOWS 1)
endif (CMAKE_SYSTEM MATCHES "Linux")

# Set AMD_HAVE_ELEMENTAL 
if (Elemental_FOUND)
  set(AMD_HAVE_ELEMENTAL 1)
endif(Elemental_FOUND)

# Set AMD_HAVE_EIGEN when we find the include directory
if (EIGEN3_INCLUDE_DIR)
  set(AMD_HAVE_EIGEN 1)
endif(EIGEN3_INCLUDE_DIR)


# Set up the configure 
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
                ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Make sure this is added to the list of -I flags
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Now to set the install files
install (DIRECTORY .
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/AMD
         PATTERN "CMake*" EXCLUDE
         PATTERN "cmake_tests" EXCLUDE
         PATTERN "config.h.in" EXCLUDE
         PATTERN "Makefile" EXCLUDE)

# Make sure you install config.h also. This is not in the source directory.
install (FILES
         ${CMAKE_CURRENT_BINARY_DIR}/config.h
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include/AMD)
