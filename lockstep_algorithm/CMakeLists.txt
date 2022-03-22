cmake_minimum_required(VERSION 3.10)

project (lockstep_algorithm
  VERSION 1
  DESCRIPTION "..."
)


# ============================================================================ #
# Compiler settings
# ============================================================================ #
set(CMAKE_C_FLAGS "-O3 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}")

message(STATUS "CMake build configuration for lock-step algorithm")
message(STATUS "  OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "    C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "    CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "")


# ============================================================================ #
# External
# ============================================================================ #

# ---------------------------------------------------------------------------- #
# BDD Packages

# Sylvan Package (Tom van Dijk - University of Twente, Netherlands)
add_subdirectory (sylvan sylvan)

# ============================================================================ #
# Examples
# ============================================================================ #
add_subdirectory (src)