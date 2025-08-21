# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Chess")

# Cesta ke knihovně SFML (relativně vůči hlavnímu CMake souboru)
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/SFML")

# Přidání include a lib cest
include_directories(${SFML_ROOT}/include)
link_directories(${SFML_ROOT}/lib)

# Global ENet configuration
set(ENET_INCLUDE_DIR "$ENV{ENET_ROOT}/include")
set(ENET_LIBRARY "$ENV{ENET_ROOT}/lib/enet.lib")


# Ensure ENet paths are valid
if(NOT EXISTS ${ENET_INCLUDE_DIR}/enet/enet.h)
    message(FATAL_ERROR "ENet include directory not found: ${ENET_INCLUDE_DIR}")
endif()
if(NOT EXISTS ${ENET_LIBRARY})
    message(FATAL_ERROR "ENet library not found: ${ENET_LIBRARY}")
endif()

# Add the include directory globally
include_directories(${ENET_INCLUDE_DIR})

# Make the library accessible globally for linking
set(ENET_LIB ${ENET_LIBRARY})


# Include sub-projects.
add_subdirectory ("Chess")


# Copy the Assets folder to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/Chess/Assets DESTINATION ${CMAKE_BINARY_DIR}/Chess)