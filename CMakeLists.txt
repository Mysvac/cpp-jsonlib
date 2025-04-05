cmake_minimum_required(VERSION 3.24)

set(jsonlib_version 1.0.0)

set(package_name mysvac-jsonlib)
set(lib_name jsonlib)
set(prev_name mysvac)

project(${package_name} 
    VERSION ${jsonlib_version}
    DESCRIPTION "A simple JSON library using C++17."
    HOMEPAGE_URL "https://github.com/Mysvac/cpp-jsonlib"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 第1步 添加库目标
add_library(${lib_name} src/jsonlib.cpp include/jsonlib.h)
# 为库起个别名
add_library(${prev_name}::${lib_name} ALIAS ${lib_name})

set_target_properties(${lib_name} PROPERTIES
    OUTPUT_NAME "mysvac-${lib_name}"
)

# win下生成动态库，添加导入导出标记
if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${lib_name}
		PRIVATE "JSONLIB_EXPORT=__declspec(dllexport)"
		INTERFACE "JSONLIB_EXPORT=__declspec(dllimport)")
endif()

# 添加包含目录
target_include_directories(${lib_name}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


# 第3步 生成Package配置文件
configure_package_config_file(
    cmake/${package_name}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}
    NO_SET_AND_CHECK_MACRO
)

# 第3步 生成package版本文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake
    VERSION ${jsonlib_version}
    COMPATIBILITY SameMajorVersion
)
# 第5步 安装配置文件
install(
FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_DATADIR}/${package_name}
)

# 第2步 安装库文件和头文件，此处是库文件目标
install(TARGETS ${lib_name} EXPORT ${package_name}-targets)
# 第2步 安装库文件和头文件，此处是头文件
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mysvac)

# 安装库文件
install(EXPORT ${package_name}-targets
        NAMESPACE ${lib_name}::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${package_name}
) # 注意此处的 DESTINATION 是包名而非命名空间或库名


if(MYSVAC_JSONLIB_ENABLE_TEST STREQUAL "ON")
    enable_testing()
    
    # ctest -R MyTest -C DEBUG --verbose
    add_executable(my_test test/test.cpp src/jsonlib.cpp)
    target_include_directories(my_test PRIVATE include)

    # 添加测试用例
    add_test(NAME MyTest COMMAND my_test)


    # ctest -R GoogleTest -C DEBUG --verbose
    find_package(GTest CONFIG REQUIRED)
    add_executable(g_test test/g_test.cpp src/jsonlib.cpp)
    target_include_directories(g_test PRIVATE include)
    target_link_libraries(g_test PRIVATE GTest::gtest GTest::gtest_main)
    add_test(NAME GoogleTest COMMAND g_test)

    add_custom_command(TARGET g_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/test/files
        ${CMAKE_CURRENT_BINARY_DIR}/files
        COMMENT "Copying files directory..."
        VERBATIM
    )

    # ctest -R GoogleTest -C DEBUG --verbose
endif()
