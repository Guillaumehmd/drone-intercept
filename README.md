# drone-intercept

Simulation of an autonomous interceptor drone that detects and intercepts a hostile drone
using its own on-board sensors, with no external guidance. Written in C++20, with a 3D
viewer built on raylib and Dear ImGui.

Work in progress: for now the build, tests and viewer are in place, and the viewer shows
fake trajectories. Physics, detection and interception come next, in `di_core`.

## Layout

- `core/`: `di_core`, the simulation library (no graphics dependency)
- `tests/`: GoogleTest unit tests
- `apps/viewer/`: `di_viewer`, the 3D viewer
- `docs/adr/`: architecture decision records

## Build

Needs CMake 3.25+, Ninja and GCC 13+ or Clang 18+. On Ubuntu:

```bash
sudo apt install build-essential cmake ninja-build \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

Then:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/apps/viewer/di_viewer
```

Other presets: `release` (optimised, with debug info) and `asan` (AddressSanitizer +
UBSan, warnings as errors, viewer not built).

Dependencies (raylib 6.0, Dear ImGui 1.92, rlImGui, GoogleTest 1.15) are fetched by CMake
at configure time and pinned to fixed versions.

## Notes

- The simulation works in ENU coordinates (East, North, Up), in metres. Only the viewer
  converts to raylib's Y-up frame.
- The simulation runs at a fixed 100 Hz step, independent of the render frame rate.
- In the viewer: right-drag to orbit, mouse wheel to zoom.

<!-- TODO: screenshot ![di_viewer](docs/img/viewer.png) -->
