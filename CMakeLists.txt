###############################################################################

# Author: BAMS

###############################################################################

# We need at least CMake 2.6.0 to compile
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

###############################################################################

# Name of the project
project(AMD)

###############################################################################

# Version
set(AMD 0.0.1)

###############################################################################

# Warn if the source and the binary directories are the same
if (CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)
  message (STATUS "Warning! In source builds clutter up source dirs")
endif (CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)

###############################################################################

# Add CMake to the module path
list (APPEND CMAKE_MODULE_PATH ${AMD_SOURCE_DIR}/CMake)

###############################################################################

# Set the install prefix
if (NOT CMAKE_INSTALL_PREFIX)
  message (STATUS "Warning! Install path not set")
  message (STATUS "Setting the install path to /usr/local")
  set (CMAKE_INSTALL_PREFIX "/usr/local/")
endif (NOT CMAKE_INSTALL_PREFIX)

###############################################################################

# Check for the presence of a C and C++ compilers
if (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
  message (FATAL "C or C++ compilers could not be found!")
endif (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)

###############################################################################

# Make sure that the users know that there are choices to build
if (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
  # Release version is being built -- exceptions are disabled 
  message (STATUS "Building the Release version")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  # Debug version is being built -- exceptions are enabled 
  message (STATUS "Building the Debug version")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  # RelWithDebInfo version is being built -- exceptions are enabled 
  message (STATUS "Building the Release version with Debug information")
else (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
  # User has not choosen anything -- so we build Release by default 
  message (STATUS "Building the Release version")
  set (CMAKE_BUILD_TYPE 
       Release 
       CACHE 
       STRING
       "Options are: None, Debug, Release, RelWithDebInfo."
       FORCE)
endif (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")

###############################################################################

# Add the main subdirectory
add_subdirectory (AMD)

###############################################################################
# 2. Find Boost with the relevant packages --- Use dynamic linking in boost!
# Without dynamic linking, it's tough to create python bindings, so watch out.
# 
# The packages we attempt to find depend on the configuration options:
#
# 0. By default, we only need random, program_options and some utlities.
# a. If BUILD_PYTHON is turned on, then we need python.
# b. If USE_ELEMENTAL is turned on, then we need mpi.

set(BOOST_LIBS_TO_CHECK random program_options)

if (USE_ELEMENTAL)
  message(STATUS "Checking for mpi as Elemental support is turned on")
  set(BOOST_LIBS_TO_CHECK ${BOOST_LIBS_TO_CHECK} mpi)
endif (USE_ELEMENTAL)

if (BUILD_PYTHON)
  message(STATUS "Checking for python as python bindings are turned on")
  set(BOOST_LIBS_TO_CHECK ${BOOST_LIBS_TO_CHECK} python)
endif (BUILD_PYTHON)

set(BOOST_ROOT $ENV{BOOST_ROOT} $ENV{HOME}/Software)
SET(BOOST_USE_STATIC_LIBS OFF)
SET(BOOST_USE_MULTITHREADED ON)
SET(BOOST_USE_STATIC_RUNTIME OFF)
set(BOOST_NO_SYSTEM_PATHS TRUE)
set(BOOST_INCLUDEDIR $BOOST_ROOT/include ) 
set(BOOST_LIBRARYDIR $BOOST_ROOT/lib ) 
set(BOOST_MIN_VERSION 1.55.0 )
find_package (Boost
              REQUIRED
              ${BOOST_LIBS_TO_CHECK})
if (Boost_FOUND)
  set (AMD_HAVE_BOOST
       1
       CACHE
       STRING
       "Enables use of Boost Libraries"
       FORCE)
  include_directories (${Boost_INCLUDE_DIRS})
  link_directories (${Boost_LIBRARY_DIRS})
endif (Boost_FOUND)

###############################################################################
# 3. Optionally, the user can build AMD with Elemental support
option (USE_ELEMENTAL "Build with Elemental Matrix support" OFF)
if (USE_ELEMENTAL)

  # 3.1 Elemental requires BLAS and LAPACK libraries
  set(BLAS_LIBRARIES $ENV{BLAS_LIBRARIES})
  set(LAPACK_LIBRARIES $ENV{LAPACK_LIBRARIES})
  if(NOT BLAS_LIBRARIES OR NOT LAPACK_LIBRARIES)
    find_package(BLAS)
    find_package(LAPACK)
  endif(NOT BLAS_LIBRARIES OR NOT LAPACK_LIBRARIES)

  if(NOT BLAS_LIBRARIES OR NOT LAPACK_LIBRARIES)
      message(FATAL_ERROR "Elemental needs BLAS and LAPACK")
  endif(NOT BLAS_LIBRARIES OR NOT LAPACK_LIBRARIES)

  # 3.2 Check for MPI 
  find_package(MPI REQUIRED)
  include_directories(${MPI_INCLUDE_PATH})

  # 3.3 If BLAS and LAPACK are present, check for Elemental
  set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
  find_package(Elemental REQUIRED)
  if (Elemental_FOUND)
    include_directories (${Elemental_INCLUDE_DIR})
    link_directories (${Elemental_LIBRARY_DIR})

    # Elemental needs c++-11 features, so add this to compile 
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
      set(CXX11_COMPILER_FLAGS "-qlanglvl=extended0x")
    else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
      set(CXX11_COMPILER_FLAGS "-std=c++11")
    endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
    message(STATUS "CXX11_COMPILER_FLAGS=${CXX11_COMPILER_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CXX11_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}")

    set (AMD_HAVE_ELEMENTAL
         1
         CACHE
         STRING
         "Enables use of Elemental Libraries"
         FORCE)

    # check if elemental build type is Hybrid
    file(STRINGS 
         "${Elemental_INCLUDE_DIR}/elemental/config.h"
         ELEMENTAL_HYBRID_BUILD_TYPE 
         REGEX 
         "^[ \t]*#define[ \t]+CMAKE_BUILD_TYPE[ \t]+\"HybridRelease\"")

    if(ELEMENTAL_HYBRID_BUILD_TYPE)
      message(STATUS 
              "Elemental was built in hybrid mode. Enabling support as well.")
      set (USE_HYBRID
           1
           CACHE
           STRING
           "Enables use of hybrid MPI/OpenMP parallelization"
           FORCE)
    endif(ELEMENTAL_HYBRID_BUILD_TYPE)
  endif (Elemental_FOUND)

  # Elemental_LIBRARY will be used for any target that uses Elemntal
  set (Elemental_LIBRARIES
       ${Elemental_LIBRARY}
       ${LAPACK_LIBRARIES}
       ${BLAS_LIBRARIES}
       ${MPI_LIBRARIES})

endif (USE_ELEMENTAL)

###############################################################################

# 4. Optionally, users can also request building of python bindings
option (USE_ELEMENTAL "Build with Elemental Matrix support" OFF)
if (BUILD_PYTHON)
  # We need to find out python support. Remember that we have already
  # checked for Boost python up at the top along with Boost.
  message (WARNING "Python support is under construction")
endif (BUILD_PYTHON)


###############################################################################

# Do we build documentation
option (BUILD_DOCS "Whether to build the AMD documentation" OFF)
if (BUILD_DOCS)
  # This checks for doxygen and latex amongst other things
  include (BuildDocs)
endif (BUILD_DOCS)

###############################################################################

# Examples
option (BUILD_EXAMPLES "Whether to build examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (BUILD_EXAMPLES)

###############################################################################

# Perf tests
option (BUILD_TESTS "Whether to build tests" OFF)
if (BUILD_TESTS)
  add_subdirectory (tests)
endif (BUILD_TESTS)

###############################################################################

# Uninstall
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

###############################################################################
