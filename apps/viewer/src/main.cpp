// 3D viewer for the drone-intercept simulation (raylib + Dear ImGui).
// Trajectories are faked until di_core provides a simulation state.

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <numbers>

#include "di/version.hpp"

namespace {

// The simulation uses ENU (x east, y north, z up, in metres). raylib is Y-up, so points
// are mapped (E, N, U) -> (E, U, -N) here and nowhere else.
constexpr float kRenderScale = 0.1f; // 10 m per unit, keeps a 3 km scene inside the far plane

struct EnuPoint {
    double east_m = 0.0;
    double north_m = 0.0;
    double up_m = 0.0;
};

Vector3 toRender(const EnuPoint& p) {
    return {static_cast<float>(p.east_m) * kRenderScale, static_cast<float>(p.up_m) * kRenderScale,
            static_cast<float>(-p.north_m) * kRenderScale};
}

// Fake circular paths for the hostile drone (target) and the interceptor.
// Remove once di_core has a real simulation state.
struct FakeWorld {
    double t_s = 0.0;
    EnuPoint target;
    EnuPoint interceptor;
};

void fakeStep(FakeWorld& w, double dt_s) {
    w.t_s += dt_s;
    const double a = 0.03 * w.t_s;
    const double b = std::numbers::pi - a;
    w.target = {800.0 * std::cos(a), 800.0 * std::sin(a), 150.0 + 50.0 * std::sin(0.1 * w.t_s)};
    w.interceptor = {400.0 * std::cos(b), 400.0 * std::sin(b), 100.0};
}

// Right-drag to rotate, wheel to zoom.
struct OrbitCamera {
    float yaw_rad = 0.8f;
    float pitch_rad = 0.5f;
    float distance = 250.0f;
    Vector3 target{0.0f, 10.0f, 0.0f};
};

void updateOrbit(OrbitCamera& orbit, Camera3D& cam, bool mouseAvailable) {
    if (mouseAvailable) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            const Vector2 d = GetMouseDelta();
            orbit.yaw_rad -= d.x * 0.005f;
            orbit.pitch_rad = std::clamp(orbit.pitch_rad + d.y * 0.005f, 0.05f, 1.5f);
        }
        orbit.distance =
            std::clamp(orbit.distance * (1.0f - GetMouseWheelMove() * 0.1f), 20.0f, 800.0f);
    }
    cam.target = orbit.target;
    cam.position = {
        orbit.target.x + orbit.distance * std::cos(orbit.pitch_rad) * std::sin(orbit.yaw_rad),
        orbit.target.y + orbit.distance * std::sin(orbit.pitch_rad),
        orbit.target.z + orbit.distance * std::cos(orbit.pitch_rad) * std::cos(orbit.yaw_rad)};
}

void pushTrail(std::deque<Vector3>& trail, Vector3 p) {
    constexpr std::size_t kMaxPoints = 600;
    trail.push_back(p);
    if (trail.size() > kMaxPoints) {
        trail.pop_front();
    }
}

void drawTrail(const std::deque<Vector3>& trail, Color c) {
    for (std::size_t i = 1; i < trail.size(); ++i) {
        DrawLine3D(trail[i - 1], trail[i], c);
    }
}

void drawEnuAxes() {
    constexpr double kLen_m = 300.0;
    const Vector3 o = toRender({});
    DrawLine3D(o, toRender({kLen_m, 0.0, 0.0}), RED);   // East
    DrawLine3D(o, toRender({0.0, kLen_m, 0.0}), GREEN); // North
    DrawLine3D(o, toRender({0.0, 0.0, kLen_m}), BLUE);  // Up
}

} // namespace

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "di_viewer");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    Camera3D camera{};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    OrbitCamera orbit;

    FakeWorld world;
    constexpr double kDt_s = 0.01;       // fixed simulation step (100 Hz)
    constexpr double kMaxFrame_s = 0.25; // don't try to catch up after a long stall
    double accumulator_s = 0.0;
    bool paused = false;
    float timeScale = 1.0f;
    int stepsLastFrame = 0;

    std::deque<Vector3> targetTrail;
    std::deque<Vector3> interceptorTrail;

    while (!WindowShouldClose()) {
        // Fixed timestep: the sim always steps by kDt_s, whatever the frame rate.
        const double frame_s = std::min(static_cast<double>(GetFrameTime()), kMaxFrame_s);
        if (!paused) {
            accumulator_s += frame_s * static_cast<double>(timeScale);
        }
        stepsLastFrame = 0;
        while (accumulator_s >= kDt_s) {
            fakeStep(world, kDt_s);
            accumulator_s -= kDt_s;
            ++stepsLastFrame;
        }
        if (stepsLastFrame > 0) {
            pushTrail(targetTrail, toRender(world.target));
            pushTrail(interceptorTrail, toRender(world.interceptor));
        }

        updateOrbit(orbit, camera, !ImGui::GetIO().WantCaptureMouse);

        BeginDrawing();
        ClearBackground(Color{18, 22, 28, 255});

        BeginMode3D(camera);
        DrawGrid(30, 10.0f); // 3 km x 3 km
        drawEnuAxes();
        DrawCube(toRender({}), 2.0f, 2.0f, 2.0f, GRAY); // ENU origin marker
        drawTrail(targetTrail, Color{230, 80, 80, 180});
        drawTrail(interceptorTrail, Color{80, 160, 230, 180});
        DrawSphere(toRender(world.target), 1.5f, RED);
        DrawSphere(toRender(world.interceptor), 1.5f, SKYBLUE);
        EndMode3D();

        rlImGuiBegin();
        ImGui::Begin("Simulation");
        ImGui::Text("di_core %.*s", static_cast<int>(di::version().size()), di::version().data());
        ImGui::Text("FPS: %d", GetFPS());
        ImGui::Text("t = %.2f s  (%d steps this frame)", world.t_s, stepsLastFrame);
        ImGui::Checkbox("Pause", &paused);
        ImGui::SliderFloat("Time scale", &timeScale, 0.1f, 10.0f, "x%.1f");
        ImGui::TextUnformatted("Right-drag: orbit | Wheel: zoom");
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
