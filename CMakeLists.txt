cmake_minimum_required(VERSION 3.20)
project(GhostRun)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)

set(SHARED_SOURCES
        src/aes.cpp
        src/steg.cpp
        src/utils.cpp
        src/loader.cpp
        src/payload.hpp
        src/payload.cpp
)

set(SHARED_INCLUDES
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external
)

# Builder executable
add_executable(GhostRunBuilder
        src/builder_main.cpp
        ${SHARED_SOURCES}
)
target_include_directories(GhostRunBuilder PRIVATE ${SHARED_INCLUDES})
target_link_libraries(GhostRunBuilder PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Runner executable
add_executable(GhostRunRunner
        src/runner_main.cpp
        ${SHARED_SOURCES}
)
target_include_directories(GhostRunRunner PRIVATE ${SHARED_INCLUDES})
target_link_libraries(GhostRunRunner PRIVATE OpenSSL::SSL OpenSSL::Crypto)
