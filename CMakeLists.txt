cmake_minimum_required(VERSION 3.24)
project(MyDesktopPDF VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /permissive- /utf-8)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(core)
add_subdirectory(infrastructure)
add_subdirectory(modules)
add_subdirectory(ui)
add_subdirectory(app)

option(MDPDF_BUILD_TESTS "Build unit tests" ON)
if(MDPDF_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
