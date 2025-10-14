#include "ui/tutorial_manager.h"
#include <cmath>

namespace towerforge::ui {

    TutorialManager::TutorialManager()
        : current_step_(TutorialStep::BuildLobby)
          , animation_time_(0.0f)
          , skip_requested_(false)
          , lobby_built_(false)
          , business_built_(false)
          , shop_built_(false)
          , stair_built_(false)
          , condo_built_(false)
          , elevator_built_(false) {
    }

    TutorialManager::~TutorialManager() = default;

    void TutorialManager::Initialize() {
        current_step_ = TutorialStep::BuildLobby;
        animation_time_ = 0.0f;
        skip_requested_ = false;
        lobby_built_ = false;
        business_built_ = false;
        shop_built_ = false;
        stair_built_ = false;
        condo_built_ = false;
        elevator_built_ = false;
    }

    void TutorialManager::Update(const float delta_time) {
        animation_time_ += delta_time;
    }

    void TutorialManager::Render() {
        RenderOverlay();
        RenderStepInfo();
        RenderProgress();
        RenderButtons();
    }

    void TutorialManager::RenderOverlay() const {
        const int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        // Draw semi-transparent overlay at top
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        constexpr int overlay_y = 20;

        // Draw background with border
        DrawRectangle(overlay_x, overlay_y, OVERLAY_WIDTH, OVERLAY_HEIGHT,
                      ColorAlpha(BLACK, 0.85f));

        // Draw gold border with pulsing effect
        const float pulse = sin(animation_time_ * 2.0f) * 0.2f + 0.8f;
        DrawRectangleLines(overlay_x, overlay_y, OVERLAY_WIDTH, OVERLAY_HEIGHT,
                           ColorAlpha(GOLD, pulse));
        DrawRectangleLines(overlay_x + 1, overlay_y + 1, OVERLAY_WIDTH - 2, OVERLAY_HEIGHT - 2,
                           ColorAlpha(GOLD, pulse * 0.5f));
    }

    void TutorialManager::RenderStepInfo() const {
        const int screen_width = GetScreenWidth();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        constexpr int overlay_y = 20;

        const std::string title = GetStepTitle();
        const std::string hint = GetStepHint();

        // Draw title
        constexpr int title_font_size = 28;
        const int title_width = MeasureText(title.c_str(), title_font_size);
        const int title_x = overlay_x + (OVERLAY_WIDTH - title_width) / 2;
        DrawText(title.c_str(), title_x, overlay_y + 20, title_font_size, GOLD);

        // Draw decorative line
        constexpr int line_width = OVERLAY_WIDTH - 80;
        const int line_x = overlay_x + 40;
        DrawRectangle(line_x, overlay_y + 55, line_width, 2, GOLD);

        // Draw hint
        constexpr int hint_font_size = 18;
        const int hint_width = MeasureText(hint.c_str(), hint_font_size);
        const int hint_x = overlay_x + (OVERLAY_WIDTH - hint_width) / 2;
        DrawText(hint.c_str(), hint_x, overlay_y + 70, hint_font_size, LIGHTGRAY);

        // Draw highlighted facility indicator
        if (const std::string required = GetRequiredFacility(); !required.empty()) {
            const std::string indicator = "[Highlight: " + required + " button]";
            constexpr int indicator_font_size = 16;
            const int indicator_width = MeasureText(indicator.c_str(), indicator_font_size);
            const int indicator_x = overlay_x + (OVERLAY_WIDTH - indicator_width) / 2;
            DrawText(indicator.c_str(), indicator_x, overlay_y + 100, indicator_font_size, SKYBLUE);
        }
    }

    void TutorialManager::RenderProgress() {
        const int screen_width = GetScreenWidth();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        constexpr int overlay_y = 20;

        constexpr int total_steps = 6;
        const int current = static_cast<int>(current_step_);

        const std::string progress_text = "[Progress: " + std::to_string(current + 1) + "/" +
                                    std::to_string(total_steps) + "]";
        constexpr int progress_font_size = 16;
        const int progress_width = MeasureText(progress_text.c_str(), progress_font_size);
        const int progress_x = overlay_x + (OVERLAY_WIDTH - progress_width) / 2;
        DrawText(progress_text.c_str(), progress_x, overlay_y + 130, progress_font_size, YELLOW);

        // Draw progress bar
        constexpr int bar_width = OVERLAY_WIDTH - 100;
        constexpr int bar_height = 10;
        const int bar_x = overlay_x + 50;
        constexpr int bar_y = overlay_y + 155;

        // Background
        DrawRectangle(bar_x, bar_y, bar_width, bar_height, ColorAlpha(DARKGRAY, 0.5f));

        // Progress fill
        const float progress = (float)(current + 1) / (float)total_steps;
        const int fill_width = (int)(bar_width * progress);
        DrawRectangle(bar_x, bar_y, fill_width, bar_height, GOLD);

        // Border
        DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, GOLD);
    }

    void TutorialManager::RenderButtons() {
        const int screen_width = GetScreenWidth();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        constexpr int overlay_y = 20;

        // Skip Tutorial button
        const int skip_x = overlay_x + OVERLAY_WIDTH - BUTTON_WIDTH - 20;
        constexpr int skip_y = overlay_y + OVERLAY_HEIGHT + 10;

        // Check hover
        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();
        const bool skip_hover = mouse_x >= skip_x && mouse_x <= skip_x + BUTTON_WIDTH &&
                                mouse_y >= skip_y && mouse_y <= skip_y + BUTTON_HEIGHT;

        // Draw button
        const Color skip_bg = skip_hover ? ColorAlpha(RED, 0.4f) : ColorAlpha(DARKGRAY, 0.3f);
        const Color skip_border = skip_hover ? RED : GRAY;
        DrawRectangle(skip_x, skip_y, BUTTON_WIDTH, BUTTON_HEIGHT, skip_bg);
        DrawRectangleLines(skip_x, skip_y, BUTTON_WIDTH, BUTTON_HEIGHT, skip_border);

        const auto skip_text = "Skip Tutorial";
        constexpr int skip_text_size = 18;
        const int skip_text_width = MeasureText(skip_text, skip_text_size);
        const int skip_text_x = skip_x + (BUTTON_WIDTH - skip_text_width) / 2;
        constexpr int skip_text_y = skip_y + (BUTTON_HEIGHT - skip_text_size) / 2;
        DrawText(skip_text, skip_text_x, skip_text_y, skip_text_size, WHITE);
    }

    bool TutorialManager::HandleInput() {
        // Check skip button click
        const int screen_width = GetScreenWidth();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        constexpr int overlay_y = 20;
        const int skip_x = overlay_x + OVERLAY_WIDTH - BUTTON_WIDTH - 20;
        constexpr int skip_y = overlay_y + OVERLAY_HEIGHT + 10;

        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (mouse_x >= skip_x && mouse_x <= skip_x + BUTTON_WIDTH &&
                mouse_y >= skip_y && mouse_y <= skip_y + BUTTON_HEIGHT) {
                skip_requested_ = true;
                return true;
            }
        }

        // ESC to skip
        if (IsKeyPressed(KEY_ESCAPE)) {
            skip_requested_ = true;
            return true;
        }

        // SPACE to advance through placeholder steps (stair and condo)
        if (IsKeyPressed(KEY_SPACE)) {
            if (current_step_ == TutorialStep::BuildStair && !stair_built_) {
                stair_built_ = true;
                AdvanceToNextStep();
            } else if (current_step_ == TutorialStep::BuildCondo && !condo_built_) {
                condo_built_ = true;
                AdvanceToNextStep();
            }
        }

        return false;
    }

    void TutorialManager::OnFacilityPlaced(const std::string& facility_type) {
        bool should_advance = false;

        switch (current_step_) {
            case TutorialStep::BuildLobby:
                if (facility_type == "Lobby" && !lobby_built_) {
                    lobby_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::BuildBusiness:
                if (facility_type == "Office" && !business_built_) {
                    business_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::BuildShop:
                if (facility_type == "Shop" && !shop_built_) {
                    shop_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::BuildStair:
                // Stairs aren't a separate facility type yet, just auto-advance for now
                if (!stair_built_) {
                    stair_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::BuildCondo:
                // Residential is not in the build menu yet, skip for now
                if (!condo_built_) {
                    condo_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::BuildElevator:
                if (facility_type == "Elevator" && !elevator_built_) {
                    elevator_built_ = true;
                    should_advance = true;
                }
                break;
            case TutorialStep::Complete:
                break;
        }

        if (should_advance) {
            AdvanceToNextStep();
        }
    }

    std::string TutorialManager::GetHighlightedFacility() const {
        switch (current_step_) {
            case TutorialStep::BuildLobby:
                return "Lobby";
            case TutorialStep::BuildBusiness:
                return "Office";
            case TutorialStep::BuildShop:
                return "Shop";
            case TutorialStep::BuildStair:
                return ""; // No stair facility type yet
            case TutorialStep::BuildCondo:
                return ""; // No residential in menu yet
            case TutorialStep::BuildElevator:
                return "Elevator";
            default:
                return "";
        }
    }

    bool TutorialManager::IsFacilityAllowed(const std::string& facility_type) const {
        const std::string required = GetRequiredFacility();
        if (required.empty()) {
            return true; // Tutorial complete, allow all
        }
        return facility_type == required;
    }

    std::string TutorialManager::GetStepTitle() const {
        switch (current_step_) {
            case TutorialStep::BuildLobby:
                return "Step 1: Build a Lobby";
            case TutorialStep::BuildBusiness:
                return "Step 2: Add a Business (Office)";
            case TutorialStep::BuildShop:
                return "Step 3: Add a Shop";
            case TutorialStep::BuildStair:
                return "Step 4: Stairs (Coming Soon!)";
            case TutorialStep::BuildCondo:
                return "Step 5: Condos (Coming Soon!)";
            case TutorialStep::BuildElevator:
                return "Step 6: Connect with Elevator";
            case TutorialStep::Complete:
                return "Tutorial Complete!";
        }
        return "Tutorial";
    }

    std::string TutorialManager::GetStepHint() const {
        switch (current_step_) {
            case TutorialStep::BuildLobby:
                return "[Hint: Select Lobby from the menu and place it]";
            case TutorialStep::BuildBusiness:
                return "[Hint: Place an Office above the Lobby]";
            case TutorialStep::BuildShop:
                return "[Hint: Place a Shop on another floor]";
            case TutorialStep::BuildStair:
                return "[Hint: Press SPACE to continue]";
            case TutorialStep::BuildCondo:
                return "[Hint: Press SPACE to continue]";
            case TutorialStep::BuildElevator:
                return "[Hint: Place an Elevator to connect floors]";
            case TutorialStep::Complete:
                return "[Congratulations! You're ready to build your tower!]";
        }
        return "";
    }

    std::string TutorialManager::GetRequiredFacility() const {
        switch (current_step_) {
            case TutorialStep::BuildLobby:
                return "Lobby";
            case TutorialStep::BuildBusiness:
                return "Office";
            case TutorialStep::BuildShop:
                return "Shop";
            case TutorialStep::BuildStair:
                return ""; // Skip - not available yet
            case TutorialStep::BuildCondo:
                return ""; // Skip - not available yet
            case TutorialStep::BuildElevator:
                return "Elevator";
            default:
                return "";
        }
    }

    void TutorialManager::AdvanceToNextStep() {
        int current = static_cast<int>(current_step_);
        current++;
        current_step_ = static_cast<TutorialStep>(current);
    }

}
