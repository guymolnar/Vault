cmake_minimum_required(VERSION 3.20)
project(Vault LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

find_package(OpenSSL REQUIRED)

set(SHARED_SOURCES
        src/aes.cpp
        src/steg.cpp
        src/utils.cpp
)

# Builder executable
add_executable(VaultBuilder
        src/builder_main.cpp
        ${SHARED_SOURCES}
)
target_include_directories(VaultBuilder PRIVATE ${CMAKE_SOURCE_DIR}/src)
# Vendored single-header libs: mark as SYSTEM so their warnings don't pollute our build.
target_include_directories(VaultBuilder SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(VaultBuilder PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Runner executable
add_executable(VaultRunner
        src/runner_main.cpp
        ${SHARED_SOURCES}
)
target_include_directories(VaultRunner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(VaultRunner SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(VaultRunner PRIVATE OpenSSL::SSL OpenSSL::Crypto)
