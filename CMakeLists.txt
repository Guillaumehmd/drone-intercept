cmake_minimum_required(VERSION 3.25)
project(drone_intercept VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DI_BUILD_TESTS  "Build unit tests"            ON)
option(DI_BUILD_VIEWER "Build raylib/ImGui viewer"   ON)
option(DI_SANITIZERS   "Enable ASan + UBSan"         OFF)
option(DI_WERROR       "Treat warnings as errors"    OFF)

# Warning flags for our own targets only, so third-party code is not affected.
add_library(di_warnings INTERFACE)
target_compile_options(di_warnings INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion
    -Wsign-conversion -Wold-style-cast -Wnon-virtual-dtor -Wdouble-promotion>
  $<$<AND:$<BOOL:${DI_WERROR}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Werror>)

if(DI_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(core)

if(DI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(DI_BUILD_VIEWER)
  add_subdirectory(apps/viewer)
endif()
