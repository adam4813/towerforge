#include "rendering/renderer.h"

namespace towerforge::rendering {
    void Renderer::Initialize(const int width, const int height, const char *title) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(width, height, title);
        SetTargetFPS(60);
    }

    void Renderer::Shutdown() {
        CloseWindow();
    }

    bool Renderer::ShouldClose() {
        return WindowShouldClose();
    }

    void Renderer::BeginFrame() {
        BeginDrawing();
    }

    void Renderer::EndFrame() {
        EndDrawing();
    }

    void Renderer::Clear(const Color color) {
        ClearBackground(color);
    }
}
