cmake_minimum_required(VERSION 3.18)
project(dbsys22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
include(ExternalProject)
include(FetchContent)
enable_testing()

set(EXECUTABLE_OUTPUT_PATH      "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH         "${PROJECT_BINARY_DIR}/lib")

set(CMAKE_COMPILE_FLAGS             "-W -Wall -pedantic -march=native")
set(CMAKE_C_FLAGS                   "${CMAKE_C_FLAGS} ${CMAKE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} ${CMAKE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG           "-g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# Catch2 - Unit testing
FetchContent_Populate(
    Catch2
    URL "https://raw.githubusercontent.com/catchorg/Catch2/v2.13.7/single_include/catch2/catch.hpp"
    URL_HASH MD5=afc1a3a08d753ec1bef9a449ff418e07
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-party/catch2/include/catch2"
    DOWNLOAD_NO_EXTRACT TRUE
)

if(APPLE)
    set(MUTABLE_URL "https://bigdata.uni-saarland.de/teaching/dbsys22/2022-12-12/mutable-macos-amd64.zip")
    set(MUTABLE_MD5 "7f79ffb7b78e2535378a559121824296")
else()
    set(MUTABLE_URL "https://bigdata.uni-saarland.de/teaching/dbsys22/2022-12-12/mutable-linux-amd64.zip")
    set(MUTABLE_MD5 "be29b3d4f023fd07c3727f35419e8303")
endif()
ExternalProject_Add(
    Mutable
    PREFIX                      "mutable"
    DOWNLOAD_NAME               "mutable.zip"
    URL                         "${MUTABLE_URL}"
    URL_MD5                     "${MUTABLE_MD5}"
    DOWNLOAD_NO_EXTRACT         0
    DOWNLOAD_EXTRACT_TIMESTAMP  FALSE
    LOG_DOWNLOAD                ON
    CONFIGURE_COMMAND           ""
    BUILD_COMMAND               ""
    INSTALL_COMMAND             ""
)
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/mutable/src/Mutable/include")
link_directories("${PROJECT_BINARY_DIR}/mutable/src/Mutable/lib")

include_directories(src)
add_subdirectory(src)
add_subdirectory(unittest)
add_subdirectory(benchmark)
