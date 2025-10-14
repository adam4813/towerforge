#include "rendering/renderer.h"

namespace towerforge::rendering {

    void Renderer::Initialize(const int width, const int height, const char* title) {
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

    void Renderer::DrawRectangle(const int x, const int y, const int width, const int height, const Color color) {
        ::DrawRectangle(x, y, width, height, color);
    }

    void Renderer::DrawCircle(const int centerX, const int centerY, const float radius, const Color color) {
        ::DrawCircle(centerX, centerY, radius, color);
    }

    void Renderer::DrawText(const char* text, const int x, const int y, const int fontSize, const Color color) {
        ::DrawText(text, x, y, fontSize, color);
    }

}
