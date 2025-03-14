cmake_minimum_required(VERSION 3.24)

set(jsonlib_VERSION 0.1.0)

project(jsonlib 
    VERSION ${jsonlib_VERSION}
    DESCRIPTION "A simple JSON library using C++17."
    HOMEPAGE_URL "https://github.com/Mysvac/cpp-jsonlib"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 第1步 生成库文件
add_library(jsonlib src/jsonlib.cpp include/jsonlib.h)
add_library(jsonlib::jsonlib ALIAS jsonlib)

if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(jsonlib
		PRIVATE "JSONLIB_EXPORT=__declspec(dllexport)"
		INTERFACE "JSONLIB_EXPORT=__declspec(dllimport)")
endif()

target_compile_features(jsonlib PUBLIC cxx_std_17)

target_include_directories(jsonlib
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(jsonlib 
PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# 第3步 生成Package配置文件
configure_package_config_file(
    cmake/jsonlibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/jsonlib-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/jsonlib
    NO_SET_AND_CHECK_MACRO
)

# 第3步 生成package版本文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/jsonlib-config-version.cmake
    VERSION ${jsonlib_VERSION}
    COMPATIBILITY SameMajorVersion
)
# 第5步 安装配置文件
install(
FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/jsonlib-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/jsonlib-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_DATADIR}/jsonlib
)

# 第2步 安装库文件和头文件，此处是库文件
install(TARGETS jsonlib EXPORT jsonlib-targets)
# 第2步 安装库文件和头文件，此处是头文件
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 导出目标
install(EXPORT jsonlib-targets
        NAMESPACE jsonlib::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/jsonlib)

