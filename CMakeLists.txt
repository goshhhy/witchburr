cmake_minimum_required(VERSION 3.8)
project(witchburr LANGUAGES CXX VERSION 1.1.6)

# Determine if we're built as a subproject (using add_subdirectory)
# or if this is the master project.
set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

# Disable in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
endif()

# Source project files
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/witchburr/witchburr.hpp
)

# Library definition
add_library(witchburr INTERFACE)
add_library(goshhhy::witchburr ALIAS witchburr)
target_sources(witchburr INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(witchburr INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(witchburr INTERFACE cxx_std_20)

# Tests
if (MASTER_PROJECT)
    option(WITCHBURR_USE_BUNDLED_CATCH "Use the embedded Catch2 submodule" OFF)
    if (WITCHBURR_USE_BUNDLED_CATCH)
        add_subdirectory(externals/catch)
    else()
        find_package(Catch2 3 REQUIRED)
    endif()

    add_executable(witchburr-tests
        tests/basic.cpp
    )
    target_include_directories(witchburr-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    target_link_libraries(witchburr-tests PRIVATE Catch2::Catch2WithMain
	    goshhhy::witchburr)
    target_compile_options(witchburr-tests PRIVATE -Wall -Wextra -Wcast-qual -pedantic -pedantic-errors -Wfatal-errors -Wno-missing-braces)
    endif()
endif()

# Export
include(GNUInstallDirs)

install(TARGETS witchburr EXPORT witchburrTargets)
install(EXPORT witchburrTargets
    NAMESPACE goshhhy::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/witchburr"
)
