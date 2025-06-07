cmake_minimum_required(VERSION 3.24)

# Read project metadata
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.private/project.json" PROJECT_INFO)
string(JSON PROJECT_NAME GET "${PROJECT_INFO}" exeName)
string(JSON PROJECT_VERSION GET "${PROJECT_INFO}" version)
string(JSON PROJECT_VERSION_STATE GET "${PROJECT_INFO}" versionState)
string(JSON PROJECT_VERSION_STD GET "${PROJECT_INFO}" versionSTD)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)

# Project settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_BUILD_TYPE Debug)

# Enable policy for install RPATH handling
cmake_policy(SET CMP0147 NEW)

# Directory structure
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Source files
set(HEADERS
    ${INC_DIR}/macros.h
    ${INC_DIR}/types.h
    ${INC_DIR}/Argx.h
    ${INC_DIR}/ARGXAddError.h
)

set(SOURCES
    ${SRC_DIR}/Argx.c
    ${SRC_DIR}/ARGXAddError.c
)

# Function to configure common target properties
function(configure_target TARGET_NAME)
    target_include_directories(${TARGET_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${INC_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:-g;-Og;-Wall;-Wextra;-Wpedantic>
        $<$<CONFIG:RELEASE>:-O3;-march=native;-DNDEBUG>
    )
    
    target_link_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:>
    )
endfunction()

# Create libraries
add_library(${PROJECT_NAME}_static STATIC ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}_shared SHARED ${SOURCES} ${HEADERS})

# Configure library targets
configure_target(${PROJECT_NAME}_static)
configure_target(${PROJECT_NAME}_shared)

# Create aliases for better namespace usage
add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}_static)
add_library(${PROJECT_NAME}::shared ALIAS ${PROJECT_NAME}_shared)

# Create executable
add_executable(${PROJECT_NAME} ${TESTS_DIR}/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}::shared)
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})
configure_target(${PROJECT_NAME})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS ${PROJECT_NAME}_static ${PROJECT_NAME}_shared ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/${PROJECT_NAME}
    ARCHIVE DESTINATION lib/${PROJECT_NAME}
    INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY ${INC_DIR}/ DESTINATION include/${PROJECT_NAME})

# Package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)
