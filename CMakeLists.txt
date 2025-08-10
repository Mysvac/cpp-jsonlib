cmake_minimum_required(VERSION 3.30)

set(prev_name mysvac)                          
set(lib_name json)                     
set(package_name "mysvac-jsonlib")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MYSVAC_JSONLIB_ENABLE_TEST)
    set(CMAKE_CXX_STANDARD 23)
endif ()

# Project declaration with metadata
project(${package_name}
    VERSION 3.0.0                                     # Semantic versioning (MAJOR.MINOR.PATCH)
    HOMEPAGE_URL "https://github.com/Mysvac/cpp-jsonlib"
    DESCRIPTION "A simple JSON library using C++20."
    LANGUAGES CXX                                     # C++ only project
)

# Source file discovery
# Recursively find all C++23 module files (.cppm and .ixx extensions)
file(GLOB_RECURSE cxx_module_files "${CMAKE_CURRENT_SOURCE_DIR}/modules/*.ixx")
# Recursively find all C/C++ header files in the `include` directory
file(GLOB_RECURSE cxx_header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# Library target creation
# Create the main library target that can be built as either static or shared
add_library(${lib_name})
# Create an alias target for namespaced usage
add_library(${prev_name}::${lib_name} ALIAS ${lib_name})

# Default use unordered_map
# If you want to use std::map instead, define M_MYSVAC_JSON_ORDERED_MAP
# target_compile_definitions(${lib_name} PUBLIC
#     M_MYSVAC_JSON_UNORDERED_MAP=1    # Define to use std::unordered_map for JSON objects
# )

# Library properties configuration
set_target_properties(${lib_name} PROPERTIES
    # CXX_MODULE_STD ON                                 # Enable C++23 standard library modules
    OUTPUT_NAME ${package_name}                       # Set the output file name
)
# Source files configuration
# Add C++23 module files as PUBLIC sources (exported for consumers)
target_sources(${lib_name} PUBLIC
        FILE_SET cxx_modules                          # Define a file set for modules
        TYPE CXX_MODULES                              # Specify this is a C++ modules file set
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules # Base directory for module resolution
        FILES ${cxx_module_files}                     # List of module files to include
)
# Add header files as INTERFACE sources (header-only, not compiled)
target_sources(${lib_name} INTERFACE
    FILE_SET cxx_headers                              # Define a file set for headers
    TYPE HEADERS                                      # Specify this is a headers file set
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include    # Base directory for header resolution
    FILES ${cxx_header_files}                        # List of header files to include
)
# Include directories configuration
# Set up include paths for both build and install configurations
target_include_directories(${lib_name} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>   # Path during build
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>         # Path after installation
)

# Dynamic library configuration (optional)
# Configure additional properties when building as a shared library
if(BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON               # Export all symbols on Windows (no explicit __declspec needed)
        VERSION ${PROJECT_VERSION}                  # Set the library version (used for .so.x.y.z on Linux)
        SOVERSION ${PROJECT_VERSION_MAJOR}          # Set the ABI version (used for .so.x on Linux)
    )
endif()

# Installation configuration
# Include standard CMake modules for installation and packaging
include(GNUInstallDirs)                             # Provides standard installation directory variables
include(CMakePackageConfigHelpers)                  # Provides functions for creating config files


# Target installation
# Install the library target and its associated file sets
install(TARGETS ${lib_name}
    EXPORT ${package_name}-targets                   # Export target for find_package() support
    FILE_SET cxx_modules
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}     # Install modules to include directory
                                                    # Note: vct prefix is already included in module paths
    FILE_SET cxx_headers
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}     # Install headers to include directory
                                                    # Note: vct prefix is already included in header paths
)

# Export targets for find_package() support
# Create and install the targets export file for package discovery
install(EXPORT ${package_name}-targets
    NAMESPACE ${prev_name}::                        # Use vct:: namespace for imported targets
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}   # Install to share/vct-test-unit/
)

# Package configuration files generation
# Generate the main config file from template
configure_package_config_file(
    cmake/${package_name}-config.cmake.in          # Input template file
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake   # Output config file
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}    # Installation destination
)

# Generate version compatibility file for find_package() version checking
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake   # Output version file
    VERSION ${PROJECT_VERSION}                     # Use project version
    COMPATIBILITY ExactVersion                     # Require exact version match (strict compatibility)
)

# Install configuration files
# Install both config and version files for package discovery
install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake       # Main config file
        ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake   # Version file
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}   # Install to share/${package_name}/
)

# Testing configuration (optional)
# Enable testing support
if(MYSVAC_JSONLIB_ENABLE_TEST)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_STANDARD 23)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt)
        enable_testing()                                # Enable CTest integration
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)    # Add test subdirectory
    endif()
endif()
