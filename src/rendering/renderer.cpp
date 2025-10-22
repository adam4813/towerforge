#include "rendering/renderer.h"

#include "ui/batch_renderer/batch_adapter.h"
#include "ui/batch_renderer/batch_renderer.h"

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
        ui::batch_renderer::BatchRenderer::BeginFrame();
    }

    void Renderer::EndFrame() {
        const size_t draw_calls = ui::batch_renderer::BatchRenderer::GetDrawCallCount();
        ui::batch_renderer::adapter::DrawText(TextFormat("Batch Draw Calls: %zu", draw_calls), 10, 10, 16,
                                              ColorAlpha(GREEN, 0.7f));
        ui::batch_renderer::BatchRenderer::EndFrame();
        ui::batch_renderer::BatchRenderer::ResetDrawCallCount();
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

    void Renderer::DrawText(const char *text, const int x, const int y, const int fontSize, const Color color) {
        ::DrawText(text, x, y, fontSize, color);
    }
}
