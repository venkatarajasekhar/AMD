find_package(PythonLibs REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/AMD
    ${PYTHON_INCLUDE_DIRS}
    ${AMD_BINARY_DIR} # for generated header files
)

set(AMD_SRCS
    module.cpp
    documentation.cpp
    Exception.cpp
    SymbolicScalarMatlab.cpp
    SymbolicMatrixMatlab.cpp
    ScalarMatrixFunc.cpp
)

add_library(AMD.so SHARED ${AMD_SRCS})

target_link_libraries(AMD.so 
                      ${AMD_LIBS} 
                      ${Boost_LIBRARIES} 
                      ${PYTHON_LIBRARIES})

set_target_properties(
    AMD.so PROPERTIES 
    COMPILE_FLAGS "-Wall -Wno-comment -Wno-unused -fPIC"
    PREFIX "" 
    SUFFIX "" 
    OUTPUT_NAME "AMD.so"
    SOVERSION "${AMD_VERSION_MAJOR}.${AMD_VERSION_MINOR}"
)

