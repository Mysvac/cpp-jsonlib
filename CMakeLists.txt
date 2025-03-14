cmake_minimum_required(VERSION 3.24)

project(jsonlib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 第1步 生成库文件
add_library(${PROJECT_NAME} STATIC src/*.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 第2步 安装库文件和头文件，此处是库文件
install(TARGETS ${PROJECT_NAME}
    EXPORT jsonlibTargets
    NAMESPACE jsonlib::
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 第2步 安装库文件和头文件，此处是头文件
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 第3步 生成版本文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    jsonlibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 第4步 生成配置文件
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/jsonlibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/jsonlibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonlib
)

# 第5步 安装配置文件
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/jsonlibConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/jsonlibConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonlib
)
