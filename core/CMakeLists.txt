add_library(di_core
  src/version.cpp
)
target_include_directories(di_core PUBLIC include)
target_compile_features(di_core PUBLIC cxx_std_20)
# Version comes from project(VERSION) in the top-level CMakeLists.txt.
target_compile_definitions(di_core PRIVATE DI_VERSION="${PROJECT_VERSION}")
target_link_libraries(di_core PRIVATE di_warnings)
add_library(di::core ALIAS di_core)
