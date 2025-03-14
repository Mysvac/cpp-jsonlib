cmake_minimum_required(VERSION 3.24)

set(jsonlib_VERSION 0.1.0)

set(package_name cpp-jsonlib)
set(lib_name jsonlib)

project(${package_name} 
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
add_library(${lib_name} src/jsonlib.cpp include/jsonlib.h)
add_library(${lib_name}::${lib_name} ALIAS ${lib_name})

if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${lib_name}
		PRIVATE "JSONLIB_EXPORT=__declspec(dllexport)"
		INTERFACE "JSONLIB_EXPORT=__declspec(dllimport)")
endif()

target_compile_features(${lib_name} PUBLIC cxx_std_17)

target_include_directories(${lib_name}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(${lib_name} 
PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# 第3步 生成Package配置文件
configure_package_config_file(
    cmake/jsonlib-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${lib_name}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}
    NO_SET_AND_CHECK_MACRO
)

# 第3步 生成package版本文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${lib_name}-config-version.cmake
    VERSION ${jsonlib_VERSION}
    COMPATIBILITY SameMajorVersion
)
# 第5步 安装配置文件
install(
FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/${lib_name}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${lib_name}-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_DATADIR}/${package_name}
)

# 第2步 安装库文件和头文件，此处是库文件
install(TARGETS ${lib_name} EXPORT ${lib_name}-targets)
# 第2步 安装库文件和头文件，此处是头文件
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 导出目标
install(EXPORT ${lib_name}-targets
        NAMESPACE ${lib_name}::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}
) # 注意此处的 DESTINATION 是包名而非命名空间或库名

