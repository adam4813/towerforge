#include "ui/info_windows.h"

namespace towerforge::ui {
    namespace {
        constexpr float WINDOW_WIDTH = 300.0f;
        constexpr float PADDING = 12.0f;
        constexpr float ROW_HEIGHT = 22.0f;
        constexpr float SECTION_GAP = 8.0f;
        constexpr int LABEL_FONT_SIZE = 13;
        constexpr int HEADER_FONT_SIZE = 14;
        constexpr int TITLE_FONT_SIZE = 16;
        constexpr float CLOSE_BUTTON_SIZE = 24.0f;
        constexpr float TITLE_BAR_HEIGHT = 32.0f;

        // Window heights - calculated based on content
        constexpr float FACILITY_WINDOW_HEIGHT = 480.0f;
        constexpr float PERSON_WINDOW_HEIGHT = 520.0f;
        constexpr float ELEVATOR_WINDOW_HEIGHT = 280.0f;

        // Helper to create a stat row with label and value
        std::unique_ptr<engine::ui::elements::Container> CreateStatRow(
            const std::string &label,
            engine::ui::elements::Text **value_out,
            const engine::ui::batch_renderer::Color &value_color = UITheme::ToEngineColor(LIGHTGRAY)) {
            using namespace engine::ui::components;
            using namespace engine::ui::elements;

            auto row = engine::ui::ContainerBuilder()
                    .Opacity(0)
                    .Size(WINDOW_WIDTH - 2 * PADDING, ROW_HEIGHT)
                    .Layout(std::make_unique<HorizontalLayout>(4.0f, Alignment::Start))
                    .Build();

            auto label_text = std::make_unique<Text>(
                0, 0, label, LABEL_FONT_SIZE, UITheme::ToEngineColor(GRAY)
            );
            row->AddChild(std::move(label_text));

            auto value_text = std::make_unique<Text>(
                0, 0, "", LABEL_FONT_SIZE, value_color
            );
            *value_out = value_text.get();
            row->AddChild(std::move(value_text));

            return row;
        }

        // Helper to create a section header
        std::unique_ptr<engine::ui::elements::Text> CreateSectionHeader(
            const std::string &text,
            const Color &color = GOLD) {
            return std::make_unique<engine::ui::elements::Text>(
                0, 0, text, HEADER_FONT_SIZE, UITheme::ToEngineColor(color)
            );
        }

        // Helper to create title bar with close button
        std::unique_ptr<engine::ui::elements::Container> CreateTitleBar(
            const std::string &title,
            engine::ui::elements::Text **title_out,
            engine::ui::elements::Button **close_button_out,
            std::function<void()> close_callback) {
            using namespace engine::ui::components;
            using namespace engine::ui::elements;

            auto title_bar = engine::ui::ContainerBuilder()
                    .Opacity(0)
                    .Size(WINDOW_WIDTH - 2 * PADDING, TITLE_BAR_HEIGHT)
                    .Layout(std::make_unique<HorizontalLayout>(4.0f, Alignment::Center))
                    .Build();

            // Title text (takes remaining space)
            auto title_text = std::make_unique<Text>(
                0, 0, title, TITLE_FONT_SIZE, UITheme::ToEngineColor(WHITE)
            );
            *title_out = title_text.get();
            title_bar->AddChild(std::move(title_text));

            // Spacer to push close button to the right
            auto spacer = engine::ui::ContainerBuilder()
                    .Opacity(0)
                    .Size(WINDOW_WIDTH - 2 * PADDING - 150 - CLOSE_BUTTON_SIZE, TITLE_BAR_HEIGHT)
                    .Build();
            title_bar->AddChild(std::move(spacer));

            // Close button - use fully qualified name to avoid conflict with towerforge::ui::Button
            auto close_btn = std::make_unique<engine::ui::elements::Button>(
                static_cast<int>(CLOSE_BUTTON_SIZE),
                static_cast<int>(CLOSE_BUTTON_SIZE),
                "X",
                LABEL_FONT_SIZE
            );
            close_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.6f)));
            close_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.8f)));
            close_btn->SetBorderColor(UITheme::ToEngineColor(GRAY));
            close_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
            close_btn->SetClickCallback([close_callback](const engine::ui::MouseEvent &event) {
                if (event.left_pressed && close_callback) {
                    close_callback();
                    return true;
                }
                return false;
            });
            *close_button_out = close_btn.get();
            title_bar->AddChild(std::move(close_btn));

            return title_bar;
        }
    }

    // ============================================================================
    // FacilityWindow implementation
    // ============================================================================

    FacilityWindow::FacilityWindow(const FacilityInfo &info)
        : info_(info) {
        BuildComponents();
        UpdateComponentValues();
    }

    void FacilityWindow::BuildComponents() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetSize(WINDOW_WIDTH, FACILITY_WINDOW_HEIGHT);
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(Color{30, 30, 40, 255}, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(GRAY));
        main_panel_->SetBorderWidth(1.0f);
        main_panel_->SetPadding(PADDING);
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(4.0f, Alignment::Start)
        );

        // Title bar with close button
        main_panel_->AddChild(CreateTitleBar("Facility Info", &title_text_, &close_button_, [this]() {
            visible_ = false;
            if (close_callback_) {
                close_callback_();
            }
        }));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(GRAY));
        divider->SetSize(WINDOW_WIDTH - 2 * PADDING, 1);
        main_panel_->AddChild(std::move(divider));

        // Stats section
        main_panel_->AddChild(CreateStatRow("Occupancy:", &occupancy_value_));
        main_panel_->AddChild(CreateStatRow("Revenue:", &revenue_value_, UITheme::ToEngineColor(GREEN)));
        main_panel_->AddChild(CreateStatRow("Satisfaction:", &satisfaction_value_));
        main_panel_->AddChild(CreateStatRow("Tenants:", &tenants_value_));

        // Status section header
        main_panel_->AddChild(CreateSectionHeader("--- Facility Status ---"));

        main_panel_->AddChild(CreateStatRow("Status:", &cleanliness_state_value_));
        main_panel_->AddChild(CreateStatRow("Cleanliness:", &cleanliness_value_));
        main_panel_->AddChild(CreateStatRow("Maintenance:", &maintenance_value_));
        main_panel_->AddChild(CreateStatRow("Status:", &maintenance_state_value_));

        // Adjacency section header
        main_panel_->AddChild(CreateSectionHeader("--- Adjacency Effects ---", GOLD));

        // Container for dynamic adjacency effects
        adjacency_container_ = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(WINDOW_WIDTH - 2 * PADDING, 80)
                .Layout(std::make_unique<VerticalLayout>(2.0f, Alignment::Start))
                .Build().release();
        main_panel_->AddChild(std::unique_ptr<Container>(adjacency_container_));

        // Button row
        auto button_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(WINDOW_WIDTH - 2 * PADDING, 60)
                .Layout(std::make_unique<VerticalLayout>(5.0f, Alignment::Start))
                .Build();

        auto button_row1 = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(WINDOW_WIDTH - 2 * PADDING, 25)
                .Layout(std::make_unique<HorizontalLayout>(10.0f, Alignment::Start))
                .Build();

        auto demolish = std::make_unique<engine::ui::elements::Button>(100, 25, "[Demolish]", LABEL_FONT_SIZE);
        demolish->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        demolish->SetTextColor(UITheme::ToEngineColor(RED));
        demolish_button_ = demolish.get();
        button_row1->AddChild(std::move(demolish));

        auto upgrade = std::make_unique<engine::ui::elements::Button>(100, 25, "[Upgrade]", LABEL_FONT_SIZE);
        upgrade->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        upgrade->SetTextColor(UITheme::ToEngineColor(YELLOW));
        upgrade_button_ = upgrade.get();
        button_row1->AddChild(std::move(upgrade));

        button_container->AddChild(std::move(button_row1));

        auto repair = std::make_unique<engine::ui::elements::Button>(210, 25, "[Repair Now]", LABEL_FONT_SIZE);
        repair->SetNormalColor(UITheme::ToEngineColor(ORANGE));
        repair->SetTextColor(UITheme::ToEngineColor(BLACK));
        repair_button_ = repair.get();
        button_container->AddChild(std::move(repair));

        main_panel_->AddChild(std::move(button_container));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void FacilityWindow::Update(const FacilityInfo &info) {
        info_ = info;
        title_text_->SetText(info.type + " - Floor " + std::to_string(info.floor));
        UpdateComponentValues();
    }

    void FacilityWindow::Update(float /*delta_time*/) {
        // No animation needed currently
    }

    void FacilityWindow::UpdateComponentValues() {
        // Update occupancy
        occupancy_value_->SetText(
            std::to_string(info_.occupancy) + "/" + std::to_string(info_.max_occupancy)
        );

        // Update revenue
        std::stringstream revenue_ss;
        revenue_ss << "$" << std::fixed << std::setprecision(0) << info_.revenue << "/hr";
        revenue_value_->SetText(revenue_ss.str());

        // Update satisfaction
        const std::string sat_emoji = GetSatisfactionEmoji(info_.satisfaction);
        satisfaction_value_->SetText(
            sat_emoji + " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%"
        );

        // Update tenants
        tenants_value_->SetText(std::to_string(info_.tenant_count) + " workers");

        // Update cleanliness state
        if (!info_.cleanliness_state.empty()) {
            Color state_color = GREEN;
            if (info_.cleanliness_state == "Dirty") {
                state_color = RED;
            } else if (info_.cleanliness_state == "Needs Cleaning") {
                state_color = YELLOW;
            }
            cleanliness_state_value_->SetText(info_.cleanliness_state);
            cleanliness_state_value_->SetColor(UITheme::ToEngineColor(state_color));
        }

        // Update cleanliness
        const std::string clean_text = info_.cleanliness_rating + " (" +
                                       std::to_string(static_cast<int>(info_.cleanliness)) + "%)";
        const Color clean_color = info_.cleanliness >= 70.0f ? GREEN : (info_.cleanliness >= 50.0f ? YELLOW : RED);
        cleanliness_value_->SetText(clean_text);
        cleanliness_value_->SetColor(UITheme::ToEngineColor(clean_color));

        // Update maintenance
        const std::string maint_text = info_.maintenance_rating + " (" +
                                       std::to_string(static_cast<int>(info_.maintenance_level)) + "%)";
        const Color maint_color = info_.maintenance_level >= 70.0f
                                      ? GREEN
                                      : (info_.maintenance_level >= 50.0f ? YELLOW : RED);
        maintenance_value_->SetText(maint_text);
        maintenance_value_->SetColor(UITheme::ToEngineColor(maint_color));

        // Update maintenance state
        if (!info_.maintenance_state.empty()) {
            Color state_color = GREEN;
            if (info_.is_broken) {
                state_color = RED;
            } else if (info_.needs_repair) {
                state_color = ORANGE;
            }
            maintenance_state_value_->SetText(info_.maintenance_state);
            maintenance_state_value_->SetColor(UITheme::ToEngineColor(state_color));
        }

        // Update adjacency effects
        adjacency_container_->ClearChildren();
        for (const auto &effect: info_.adjacency_effects) {
            Color effect_color = LIGHTGRAY;
            if (effect.find("+") != std::string::npos) {
                effect_color = GREEN;
            } else if (effect.find("-") != std::string::npos) {
                effect_color = ORANGE;
            }

            auto effect_text = std::make_unique<engine::ui::elements::Text>(
                0, 0, effect, LABEL_FONT_SIZE, UITheme::ToEngineColor(effect_color)
            );
            adjacency_container_->AddChild(std::move(effect_text));
        }

        // Update repair button visibility/state
        if (info_.needs_repair || info_.is_broken) {
            const Color repair_color = info_.is_broken ? RED : ORANGE;
            const std::string button_text = info_.is_broken ? "[Emergency Repair]" : "[Repair Now]";
            repair_button_->SetNormalColor(UITheme::ToEngineColor(repair_color));
            repair_button_->SetTextColor(UITheme::ToEngineColor(BLACK));
            repair_button_->SetLabel(button_text);
        }

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void FacilityWindow::Render() const {
        if (!visible_ || !main_panel_) return;
        main_panel_->Render();
    }

    bool FacilityWindow::ProcessMouseEvent(const engine::ui::MouseEvent &event) {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void FacilityWindow::SetPosition(float x, float y) {
        if (main_panel_) {
            main_panel_->SetRelativePosition(x, y);
        }
    }

    float FacilityWindow::GetWidth() const {
        if (main_panel_) {
            return main_panel_->GetWidth();
        }
        return 0;
    }

    float FacilityWindow::GetHeight() const {
        if (main_panel_) {
            return main_panel_->GetHeight();
        }
        return 0;
    }

    std::string FacilityWindow::GetSatisfactionEmoji(const float satisfaction) {
        if (satisfaction >= 80) return ":)";
        if (satisfaction >= 60) return ":|";
        if (satisfaction >= 40) return ":/";
        return ":(";
    }

    // ============================================================================
    // PersonWindow implementation
    // ============================================================================

    PersonWindow::PersonWindow(const PersonInfo &info)
        : info_(info) {
        BuildComponents();
        UpdateComponentValues();
    }

    void PersonWindow::BuildComponents() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetSize(WINDOW_WIDTH, PERSON_WINDOW_HEIGHT);
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(Color{30, 30, 40, 255}, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(GRAY));
        main_panel_->SetBorderWidth(1.0f);
        main_panel_->SetPadding(PADDING);
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(4.0f, Alignment::Start)
        );

        // Title bar with close button
        main_panel_->AddChild(CreateTitleBar("Person Info", &title_text_, &close_button_, [this]() {
            visible_ = false;
            if (close_callback_) {
                close_callback_();
            }
        }));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(GRAY));
        divider->SetSize(WINDOW_WIDTH - 2 * PADDING, 1);
        main_panel_->AddChild(std::move(divider));

        // Basic info
        main_panel_->AddChild(CreateStatRow("Type:", &type_value_));
        main_panel_->AddChild(CreateStatRow("Profile:", &archetype_value_));

        // Staff section
        main_panel_->AddChild(CreateSectionHeader("--- Staff Info ---", GOLD));
        main_panel_->AddChild(CreateStatRow("Role:", &role_value_));
        main_panel_->AddChild(CreateStatRow("Status:", &duty_value_));
        main_panel_->AddChild(CreateStatRow("Shift:", &shift_value_));

        // Status section
        main_panel_->AddChild(CreateStatRow("Status:", &status_value_));
        main_panel_->AddChild(CreateStatRow("State:", &state_value_));
        main_panel_->AddChild(CreateStatRow("Current:", &current_floor_value_));
        main_panel_->AddChild(CreateStatRow("Destination:", &dest_floor_value_));
        main_panel_->AddChild(CreateStatRow("Wait Time:", &wait_time_value_));

        // Needs section
        main_panel_->AddChild(CreateSectionHeader("--- Visitor Needs ---", YELLOW));
        main_panel_->AddChild(CreateStatRow("Hunger:", &hunger_value_));
        main_panel_->AddChild(CreateStatRow("Entertainment:", &entertainment_value_));
        main_panel_->AddChild(CreateStatRow("Comfort:", &comfort_value_));
        main_panel_->AddChild(CreateStatRow("Shopping:", &shopping_value_));

        // Satisfaction
        main_panel_->AddChild(CreateStatRow("Satisfaction:", &satisfaction_value_));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void PersonWindow::UpdateComponentValues() {
        // Update type
        type_value_->SetText(info_.npc_type);
        type_value_->SetColor(UITheme::ToEngineColor(SKYBLUE));

        // Update archetype
        if (info_.has_needs && !info_.visitor_archetype.empty()) {
            archetype_value_->SetText(info_.visitor_archetype);
            archetype_value_->SetColor(UITheme::ToEngineColor(GOLD));
        }

        // Update staff info
        if (info_.is_staff) {
            role_value_->SetText(info_.staff_role);
            duty_value_->SetText(info_.on_duty ? "On Duty" : "Off Duty");
            duty_value_->SetColor(UITheme::ToEngineColor(info_.on_duty ? GREEN : GRAY));
            shift_value_->SetText(info_.shift_hours);
        }

        // Update status
        status_value_->SetText(info_.status);
        status_value_->SetColor(UITheme::ToEngineColor(GOLD));
        state_value_->SetText(info_.state);
        current_floor_value_->SetText("Floor " + std::to_string(info_.current_floor));
        dest_floor_value_->SetText("Floor " + std::to_string(info_.destination_floor));

        // Update wait time
        if (!info_.is_staff || info_.wait_time > 0) {
            std::stringstream wait_ss;
            wait_ss << std::fixed << std::setprecision(0) << info_.wait_time << "s";
            wait_time_value_->SetText(wait_ss.str());
            wait_time_value_->SetColor(UITheme::ToEngineColor(info_.wait_time > 30 ? RED : LIGHTGRAY));
        }

        // Update needs
        if (info_.has_needs) {
            auto get_need_color = [](const float need) {
                if (need < 30.0f) return GREEN;
                if (need < 60.0f) return YELLOW;
                return RED;
            };

            hunger_value_->SetText(std::to_string(static_cast<int>(info_.hunger_need)) + "%");
            hunger_value_->SetColor(UITheme::ToEngineColor(get_need_color(info_.hunger_need)));
            entertainment_value_->SetText(std::to_string(static_cast<int>(info_.entertainment_need)) + "%");
            entertainment_value_->SetColor(UITheme::ToEngineColor(get_need_color(info_.entertainment_need)));
            comfort_value_->SetText(std::to_string(static_cast<int>(info_.comfort_need)) + "%");
            comfort_value_->SetColor(UITheme::ToEngineColor(get_need_color(info_.comfort_need)));
            shopping_value_->SetText(std::to_string(static_cast<int>(info_.shopping_need)) + "%");
            shopping_value_->SetColor(UITheme::ToEngineColor(get_need_color(info_.shopping_need)));
        }

        // Update satisfaction
        if (!info_.is_staff) {
            const std::string sat_emoji = GetSatisfactionEmoji(info_.satisfaction);
            satisfaction_value_->SetText(sat_emoji + " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%");
        }
    }

    void PersonWindow::Update(const PersonInfo &info) {
        info_ = info;
        title_text_->SetText(info.name);
        UpdateComponentValues();
    }

    void PersonWindow::Update(float /*delta_time*/) {
        // No animation needed currently
    }

    void PersonWindow::Render() const {
        if (!visible_ || !main_panel_) return;
        main_panel_->Render();
    }

    bool PersonWindow::ProcessMouseEvent(const engine::ui::MouseEvent &event) {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void PersonWindow::SetPosition(float x, float y) {
        if (main_panel_) {
            main_panel_->SetRelativePosition(x, y);
        }
    }

    float PersonWindow::GetWidth() const {
        if (main_panel_) {
            return main_panel_->GetWidth();
        }
        return 0;
    }

    float PersonWindow::GetHeight() const {
        if (main_panel_) {
            return main_panel_->GetHeight();
        }
        return 0;
    }

    std::string PersonWindow::GetSatisfactionEmoji(const float satisfaction) {
        if (satisfaction >= 80) return ":)";
        if (satisfaction >= 60) return ":|";
        if (satisfaction >= 40) return ":/";
        return ":(";
    }

    // ============================================================================
    // ElevatorWindow implementation
    // ============================================================================

    ElevatorWindow::ElevatorWindow(const ElevatorInfo &info)
        : info_(info) {
        BuildComponents();
        UpdateComponentValues();
    }

    void ElevatorWindow::BuildComponents() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetSize(WINDOW_WIDTH, ELEVATOR_WINDOW_HEIGHT);
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(Color{30, 30, 40, 255}, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(GRAY));
        main_panel_->SetBorderWidth(1.0f);
        main_panel_->SetPadding(PADDING);
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(4.0f, Alignment::Start)
        );

        // Title bar with close button
        main_panel_->AddChild(CreateTitleBar("Elevator Info", &title_text_, &close_button_, [this]() {
            visible_ = false;
            if (close_callback_) {
                close_callback_();
            }
        }));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(GRAY));
        divider->SetSize(WINDOW_WIDTH - 2 * PADDING, 1);
        main_panel_->AddChild(std::move(divider));

        // Stats
        main_panel_->AddChild(CreateStatRow("Current Floor:", &current_floor_value_));
        main_panel_->AddChild(CreateStatRow("Occupancy:", &occupancy_value_));
        main_panel_->AddChild(CreateStatRow("Next Stop:", &next_stop_value_));
        main_panel_->AddChild(CreateStatRow("Queue Length:", &queue_length_value_));

        // Container for dynamic queue items
        queue_container_ = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(WINDOW_WIDTH - 2 * PADDING, 100)
                .Layout(std::make_unique<VerticalLayout>(2.0f, Alignment::Start))
                .Build().release();
        main_panel_->AddChild(std::unique_ptr<Container>(queue_container_));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void ElevatorWindow::UpdateComponentValues() {
        // Update current floor and direction
        const std::string current = std::to_string(info_.current_floor) + " " + info_.direction;
        current_floor_value_->SetText(current);

        // Update occupancy
        const std::string occupancy = std::to_string(info_.occupancy) + "/" + std::to_string(info_.max_occupancy);
        occupancy_value_->SetText(occupancy);

        // Update next stop
        const std::string next_stop = "Floor " + std::to_string(info_.next_stop);
        next_stop_value_->SetText(next_stop);

        // Update queue length
        queue_length_value_->SetText(std::to_string(info_.queue.size()));

        // Update queue items
        queue_container_->ClearChildren();
        for (const auto &[floor, waiting]: info_.queue) {
            const std::string queue_text = "- Floor " + std::to_string(floor) + ": " + std::to_string(waiting) +
                                           " waiting";
            auto queue_item = std::make_unique<engine::ui::elements::Text>(
                0, 0, queue_text, LABEL_FONT_SIZE, UITheme::ToEngineColor(GRAY)
            );
            queue_container_->AddChild(std::move(queue_item));
        }

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void ElevatorWindow::Update(const ElevatorInfo &info) {
        info_ = info;
        title_text_->SetText("ELEVATOR #" + std::to_string(info.id));
        UpdateComponentValues();
    }

    void ElevatorWindow::Update(float /*delta_time*/) {
        // No animation needed currently
    }

    void ElevatorWindow::Render() const {
        if (!visible_ || !main_panel_) return;
        main_panel_->Render();
    }

    bool ElevatorWindow::ProcessMouseEvent(const engine::ui::MouseEvent &event) {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void ElevatorWindow::SetPosition(float x, float y) {
        if (main_panel_) {
            main_panel_->SetRelativePosition(x, y);
        }
    }

    float ElevatorWindow::GetWidth() const {
        if (main_panel_) {
            return main_panel_->GetWidth();
        }
        return 0;
    }

    float ElevatorWindow::GetHeight() const {
        if (main_panel_) {
            return main_panel_->GetHeight();
        }
        return 0;
    }
}
