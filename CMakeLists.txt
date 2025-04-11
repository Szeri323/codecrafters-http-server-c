cmake_minimum_required(VERSION 3.13)

project(codecrafters-http-server)


file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(http-server ${SOURCE_FILES})

find_package(ZLIB REQUIRED)

if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(http-server ${ZLIB_LIBRARIES})
else()
    message(FATAL_ERROR "Biblioteka zlib nie została znaleziona.")
endif()
