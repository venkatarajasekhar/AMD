find_package(PythonLibs REQUIRED)

include_directories (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/AMD
    ${PYTHON_INCLUDE_DIRS}
)

set(AMD_SRCS
    module.cpp
    documentation.cpp
    SymbolicScalarMatlab.cpp
    SymbolicMatrixMatlab.cpp
)

add_library(AMD.so SHARED ${AMD_SRCS})
target_link_libraries(AMD.so 
                      ${AMD_LIBS} 
                      ${Boost_LIBRARIES} 
                      ${PYTHON_LIBRARIES})
set_target_properties(AMD.so 
                      PROPERTIES COMPILE_FLAGS
"-Wall -Wno-comment -fPIC"
)
set_target_properties(AMD.so PROPERTIES PREFIX "" )


IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    INSTALL (
        FILES ${PROJECT_BINARY_DIR}/python/amd.so.dylib
        DESTINATION lib
        RENAME AMD.so
    )
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    INSTALL (
        TARGETS AMD.so
        DESTINATION lib
    )
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

