include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(di_core_tests
  test_smoke.cpp
)
target_link_libraries(di_core_tests PRIVATE di::core di_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(di_core_tests)
