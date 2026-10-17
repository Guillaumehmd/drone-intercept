include(FetchContent)

# raylib has its own CMake build.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM)

# ImGui and rlImGui have no CMakeLists.txt: they are only downloaded and built by di_imgui.
# rlImGui has no tags, so it is pinned to a commit that works with ImGui 1.92.
FetchContent_Declare(imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_Declare(rlimgui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG        1550009359ad975927f7f0e4a3f47e3f27123ea9)

FetchContent_MakeAvailable(raylib imgui rlimgui)

add_library(di_imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)
target_include_directories(di_imgui SYSTEM PUBLIC ${imgui_SOURCE_DIR} ${rlimgui_SOURCE_DIR})
target_link_libraries(di_imgui PUBLIC raylib)

add_executable(di_viewer src/main.cpp)
target_link_libraries(di_viewer PRIVATE di::core di_imgui di_warnings)
