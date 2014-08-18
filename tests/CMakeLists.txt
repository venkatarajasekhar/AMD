cmake_minimum_required (VERSION 2.6)

# Add the AMD source directory to be in the include directory path.
# Add the AMD binary directory to be in the include directory path.
# This is done so that the generated header files can also be found.
include_directories (BEFORE ${AMD_SOURCE_DIR} ${AMD_BINARY_DIR})

# Create a custom target for all the executables
add_custom_target (cxx_tests ALL)

add_executable (TestSymbolicMatrices TestSymbolicMatrices.cpp)
add_dependencies (cxx_tests TestSymbolicMatrices)

add_executable (TestMatrixMatrixFunc TestMatrixMatrixFunc.cpp)
add_dependencies (cxx_tests TestMatrixMatrixFunc)

add_executable (SymbolicCalculator SymbolicCalculator.cpp)
add_dependencies (cxx_tests SymbolicCalculator)

add_executable (SymbolicSample SymbolicSample.cpp)
add_dependencies (cxx_tests SymbolicSample)

add_executable (TaylorSample TaylorSample.cpp)
add_dependencies (cxx_tests TaylorSample)
##############################################################################
# This is the right way to create an executable with Elemental
if (USE_ELEMENTAL)
  add_executable (TestElementalAdaptor TestElementalAdaptor.cpp)
  add_dependencies (cxx_tests TestElementalAdaptor)
  add_executable (ElementalSample ElementalSample.cpp)
  add_dependencies (cxx_tests ElementalSample)

  # You have to add link to Elemental_LIBRARIES for all executables!  
  target_link_libraries (TestElementalAdaptor "-lm")
  target_link_libraries (TestElementalAdaptor ${Elemental_LIBRARIES})
  target_link_libraries (ElementalSample "-lm")
  target_link_libraries (ElementalSample ${Elemental_LIBRARIES})
  target_link_libraries (TestMatrixMatrixFunc ${Elemental_LIBRARIES})
  target_link_libraries (SymbolicCalculator   ${Elemental_LIBRARIES})
  target_link_libraries (TestSymbolicMatrices ${Elemental_LIBRARIES})
  target_link_libraries (SymbolicSample ${Elemental_LIBRARIES})
  target_link_libraries (TaylorSample ${Elemental_LIBRARIES})

  # Use MPI to compile TestElementalAdaptor
  if (MPI_COMPILE_FLAGS)
    set_target_properties(TestElementalAdaptor PROPERTIES 
                          COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  endif (MPI_COMPILE_FLAGS)
  if (MPI_LINK_FLAGS)
    set_target_properties(TestElementalAdaptor PROPERTIES 
                          LINK_FLAGS "${MPI_COMPILE_FLAGS}")
  endif (MPI_LINK_FLAGS)

endif (USE_ELEMENTAL)


##############################################################################
