cmake_minimum_required(VERSION 3.10)
project(connect4_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add vcpkg toolchain file if using vcpkg (Windows)
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Find packages
find_package(jsoncpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Add source files
add_executable(${PROJECT_NAME}
    server.cpp
    Solver.cpp
    Position.cpp
)

# Add header files
set(HEADERS
    Position.hpp
    Solver.hpp
    MoveSorter.hpp
    TranspositionTable.hpp
    OpeningBook.hpp
    httplib.h
    json.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    JsonCpp::JsonCpp
    nlohmann_json::nlohmann_json
)

# Add compiler definitions for Windows
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _WIN32_WINNT=0x0601
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Add httplib as header-only library
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# Set compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Install vcpkg dependencies (you need to run this first):
# vcpkg install nlohmann-json:x64-windows

# Find required packages
find_package(Threads REQUIRED)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against required libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif() 