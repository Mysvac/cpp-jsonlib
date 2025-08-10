cmake_minimum_required(VERSION 3.30)

set(prev_name mysvac)                          
set(lib_name jsonlib)
set(package_name "${prev_name}-${lib_name}")

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
add_library(${lib_name} INTERFACE)
# Create an alias target for namespaced usage
add_library(${prev_name}::${lib_name} ALIAS ${lib_name})

# Library properties configuration
set_target_properties(${lib_name} PROPERTIES
    OUTPUT_NAME ${package_name}                       # Set the output file name
)
# Add header files as INTERFACE sources (header-only, not compiled)
target_sources(${lib_name} INTERFACE
    FILE_SET mysvac_json_cxx_headers                 # Define a file set for headers
    TYPE HEADERS                                     # Specify this is a headers file set
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${cxx_header_files}
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules
    FILES ${cxx_module_files}
)
# Include directories configuration
# Set up include paths for both build and install configurations
target_include_directories(${lib_name} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>   # Path during build
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>         # Path after installation
)

# Installation configuration
# Include standard CMake modules for installation and packaging
include(GNUInstallDirs)                             # Provides standard installation directory variables
include(CMakePackageConfigHelpers)                  # Provides functions for creating config files

# Target installation
# Install the library target and its associated file sets
install(TARGETS ${lib_name}
    EXPORT ${package_name}-targets                   # Export target for find_package() support
    FILE_SET mysvac_json_cxx_headers
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
    COMPATIBILITY SameMinorVersion                 # Require exact version match (strict compatibility)
)

# Install configuration files
# Install both config and version files for package discovery
install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake       # Main config file
        ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake   # Version file
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}   # Install to share/${package_name}/
)

###################
# utilities for testing or direct usage
function(target_link_mysvac_jsonlib target visibility)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target '${target}' does not exist!")
    endif()
    # add module target
    target_link_libraries(${target} ${visibility} mysvac::jsonlib)
    target_sources(${target} ${visibility}
            FILE_SET mysvac_json_cxx_modules
            TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/modules
            FILES ${cxx_module_files}
    )
endfunction()
###################


# Testing configuration (optional)
# Enable testing support
if(MYSVAC_JSON_ENABLE_TEST)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt)
        enable_testing()                                # Enable CTest integration
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)    # Add test subdirectory
    endif()
endif()

