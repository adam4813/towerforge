#include "rendering/renderer.h"

namespace towerforge {
namespace rendering {

void Renderer::Initialize(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

void Renderer::Shutdown() {
    CloseWindow();
}

bool Renderer::ShouldClose() const {
    return WindowShouldClose();
}

void Renderer::BeginFrame() {
    BeginDrawing();
}

void Renderer::EndFrame() {
    EndDrawing();
}

void Renderer::Clear(Color color) {
    ClearBackground(color);
}

void Renderer::DrawRectangle(int x, int y, int width, int height, Color color) {
    ::DrawRectangle(x, y, width, height, color);
}

void Renderer::DrawCircle(int centerX, int centerY, float radius, Color color) {
    ::DrawCircle(centerX, centerY, radius, color);
}

void Renderer::DrawText(const char* text, int x, int y, int fontSize, Color color) {
    ::DrawText(text, x, y, fontSize, color);
}

} // namespace rendering
} // namespace towerforge
