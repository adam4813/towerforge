#include "ui/placement_system.h"
#include "ui/ui_theme.h"
#include "rendering/camera.h"
#include "core/command.hpp"
#include <iostream>
#include <sstream>

#include "ui/tooltip.h"

import engine;

namespace towerforge::ui {
    PlacementSystem::PlacementSystem(core::TowerGrid &grid, core::FacilityManager &facility_mgr, BuildMenu &build_menu)
        : grid_(grid)
          , facility_mgr_(facility_mgr)
          , build_menu_(build_menu)
          , camera_(nullptr)
          , hover_floor_(-1)
          , hover_column_(-1)
          , hover_valid_(false)
          , command_history_(50) // Max 50 actions in history
          , tooltip_manager_(nullptr) {
    }

    PlacementSystem::~PlacementSystem() = default;

    void PlacementSystem::Update(const float delta_time) {
        // Update construction progress
        for (auto it = constructions_in_progress_.begin(); it != constructions_in_progress_.end();) {
            it->build_time_elapsed += delta_time;

            if (it->IsComplete()) {
                // Construction complete - remove from list
                it = constructions_in_progress_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void PlacementSystem::Render(const int grid_offset_x, const int grid_offset_y, const int cell_width,
                                 const int cell_height) {
        // Get mouse position and convert to world coordinates if camera is set
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();

        if (camera_ != nullptr) {
            float world_x, world_y;
            camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);
            mouse_x = static_cast<int>(world_x);
            mouse_y = static_cast<int>(world_y);
        }

        // Convert to grid coordinates
        int floor, column;
        if (MouseToGrid(mouse_x, mouse_y, grid_offset_x, grid_offset_y, cell_width, cell_height, floor, column)) {
            hover_floor_ = floor;
            hover_column_ = column;


            if (const int selected = build_menu_.GetSelectedFacility(); selected >= 0) {
                const auto &types = build_menu_.GetFacilityTypes();
                const auto &facility_type = types[selected];

                // Check if placement is valid (will be checked with funds in HandleClick)
                hover_valid_ = grid_.IsSpaceAvailable(floor, column, facility_type.width);
            } else {
                hover_valid_ = false;
            }
        } else {
            hover_floor_ = -1;
            hover_column_ = -1;
            hover_valid_ = false;
        }

        // Render placement preview
        if (const int selected = build_menu_.GetSelectedFacility();
            selected >= 0 && hover_floor_ >= 0 && hover_column_ >= 0) {
            const auto &types = build_menu_.GetFacilityTypes();
            const auto &facility_type = types[selected];

            // Draw ghost preview
            const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
            const int x = grid_offset_x + hover_column_ * cell_width;
            const int y = ground_floor_screen_y - (hover_floor_ * cell_height);

            const Color preview_color = hover_valid_ ? ColorAlpha(GREEN, 0.3f) : ColorAlpha(RED, 0.3f);
            const Color outline_color = hover_valid_ ? GREEN : RED;

            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(x), static_cast<float>(y),
                                      static_cast<float>(facility_type.width * cell_width), static_cast<float>(cell_height)),
                UITheme::ToEngineColor(preview_color)
            );

            // Draw border using 4 lines
            const auto border_col = UITheme::ToEngineColor(outline_color);
            const float width_f = static_cast<float>(facility_type.width * cell_width);
            const float height_f = static_cast<float>(cell_height);
            engine::ui::BatchRenderer::SubmitLine(x, y, x + width_f, y, 1.0f, border_col);
            engine::ui::BatchRenderer::SubmitLine(x + width_f, y, x + width_f, y + height_f, 1.0f, border_col);
            engine::ui::BatchRenderer::SubmitLine(x + width_f, y + height_f, x, y + height_f, 1.0f, border_col);
            engine::ui::BatchRenderer::SubmitLine(x, y + height_f, x, y, 1.0f, border_col);

            // Draw icon
            engine::ui::BatchRenderer::SubmitText(facility_type.icon, static_cast<float>(x + 5), static_cast<float>(y + 5),
                                                  20, UITheme::ToEngineColor(WHITE));

            // Draw cost indicator
            if (hover_valid_) {
                const std::string cost_text = "$" + std::to_string(facility_type.cost);
                engine::ui::BatchRenderer::SubmitText(cost_text, static_cast<float>(x + 5),
                                                      static_cast<float>(y + cell_height - 20),
                                                      12, UITheme::ToEngineColor(GREEN));
            } else {
                engine::ui::BatchRenderer::SubmitText("INVALID", static_cast<float>(x + 5),
                                                      static_cast<float>(y + cell_height - 20),
                                                      12, UITheme::ToEngineColor(RED));
            }
        }


        // Render construction progress
        const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
        for (const auto &construction: constructions_in_progress_) {
            const int x = grid_offset_x + construction.column * cell_width;
            const int y = ground_floor_screen_y - (construction.floor * cell_height);

            // Draw construction overlay
            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(x), static_cast<float>(y),
                                      static_cast<float>(construction.width * cell_width), static_cast<float>(cell_height)),
                UITheme::ToEngineColor(ColorAlpha(ORANGE, 0.4f))
            );

            // Draw progress bar
            const int bar_width = construction.width * cell_width - 10;
            constexpr int bar_height = 8;
            const int bar_x = x + 5;
            const int bar_y = y + cell_height / 2 - bar_height / 2;

            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(bar_x), static_cast<float>(bar_y),
                                      static_cast<float>(bar_width), static_cast<float>(bar_height)),
                UITheme::ToEngineColor(DARKGRAY)
            );
            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(bar_x), static_cast<float>(bar_y),
                                      static_cast<float>(bar_width * construction.GetProgress()),
                                      static_cast<float>(bar_height)),
                UITheme::ToEngineColor(YELLOW)
            );

            // Draw construction text
            const std::string progress_text = "Building... " + std::to_string(static_cast<int>(construction.GetProgress() * 100)) + "%";
            engine::ui::BatchRenderer::SubmitText(progress_text, static_cast<float>(x + 5), static_cast<float>(y + 5),
                                                  10, UITheme::ToEngineColor(WHITE));
        }
    }

    int PlacementSystem::HandleClick(const int mouse_x, const int mouse_y,
                                     const int grid_offset_x, const int grid_offset_y,
                                     const int cell_width, const int cell_height,
                                     const float current_funds) {
        int floor, column;
        if (!MouseToGrid(mouse_x, mouse_y, grid_offset_x, grid_offset_y,
                         cell_width, cell_height, floor, column)) {
            return 0;
        }

        if (const int selected = build_menu_.GetSelectedFacility(); selected >= 0) {
            if (float funds = current_funds; PlaceFacility(floor, column, selected, funds)) {
                return static_cast<int>(funds - current_funds); // Negative (cost)
            }
        }


        return 0;
    }

    bool PlacementSystem::ExecuteDemolish(const int floor, const int column, float &funds) {
        return DemolishFacility(floor, column, funds);
    }

    bool PlacementSystem::HandleKeyboard() {
        // Ctrl+Z for undo
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
            // Note: funds adjustment handled by the game logic
            return true; // Signal that undo was requested
        }

        // Ctrl+Y for redo
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y)) {
            // Note: funds adjustment handled by the game logic
            return true; // Signal that redo was requested
        }

        return false;
    }

    void PlacementSystem::UpdateTooltips(const int mouse_x, const int mouse_y, const int grid_offset_x,
                                         const int grid_offset_y,
                                         const int cell_width, const int cell_height, const float current_funds) const {
        if (!tooltip_manager_) {
            return;
        }

        // Calculate grid position with inverted Y coordinate system
        const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
        const int grid_x = (mouse_x - grid_offset_x) / cell_width;
        const int grid_y = -(mouse_y - ground_floor_screen_y) / cell_height;

        // Check if hovering over grid
        if (grid_x >= 0 && grid_x < grid_.GetColumnCount() && grid_y >= 0 && grid_y < grid_.GetFloorCount()) {
            const int screen_x = grid_offset_x + grid_x * cell_width;
            const int screen_y = ground_floor_screen_y - (grid_y * cell_height);

            std::stringstream tooltip_text;

            if (const int selected = build_menu_.GetSelectedFacility();
                selected >= 0 && selected < static_cast<int>(build_menu_.GetFacilityTypes().size())) {
                const auto &facility = build_menu_.GetFacilityTypes()[selected];

                tooltip_text << "Place " << facility.name << "\n";
                tooltip_text << "Cost: $" << facility.cost << "\n";
                tooltip_text << "Floor: " << grid_y << ", Column: " << grid_x;

                // Check if placement is valid
                if (const bool can_afford = current_funds >= facility.cost; !can_afford) {
                    tooltip_text << "\n[INSUFFICIENT FUNDS]";
                }

                // Check if space is available
                bool space_available = true;
                for (int i = 0; i < facility.width; i++) {
                    if (grid_x + i >= grid_.GetColumnCount() || grid_.IsOccupied(grid_y, grid_x + i)) {
                        space_available = false;
                        break;
                    }
                }

                if (!space_available) {
                    tooltip_text << "\n[SPACE NOT AVAILABLE]";
                }
            } else {
                tooltip_text << "Floor: " << grid_y << ", Column: " << grid_x << "\n";
                tooltip_text << "Select a facility to build";
            }


            if (!tooltip_text.str().empty()) {
                const Tooltip tooltip(tooltip_text.str());
                tooltip_manager_->ShowTooltip(tooltip, screen_x, screen_y, cell_width, cell_height);
                return;
            }
        }

        tooltip_manager_->HideTooltip();
    }

    bool PlacementSystem::Undo(float &funds) {
        return command_history_.Undo(funds);
    }

    bool PlacementSystem::Redo(float &funds) {
        return command_history_.Redo(funds);
    }

    bool PlacementSystem::CanUndo() const {
        return command_history_.CanUndo();
    }

    bool PlacementSystem::CanRedo() const {
        return command_history_.CanRedo();
    }

    bool PlacementSystem::MouseToGrid(const int mouse_x, const int mouse_y,
                                      const int grid_offset_x, const int grid_offset_y,
                                      const int cell_width, const int cell_height,
                                      int &out_floor, int &out_column) const {
        // Convert mouse position to grid coordinates with inverted Y axis
        // Ground floor (0) is at center, floors build upward (decreasing Y)
        const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;

        const int rel_x = mouse_x - grid_offset_x;
        const int rel_y = mouse_y - ground_floor_screen_y;

        if (rel_x < 0) {
            return false;
        }

        out_column = rel_x / cell_width;
        // Invert Y: negative rel_y means above ground (positive floors), positive rel_y means below ground (negative floors in future)
        out_floor = -rel_y / cell_height;

        // Check bounds
        if (out_floor < 0 || out_floor >= grid_.GetFloorCount() ||
            out_column < 0 || out_column >= grid_.GetColumnCount()) {
            return false;
        }

        return true;
    }

    bool PlacementSystem::IsPlacementValid(const int floor, const int column, const int width,
                                           const float current_funds, const int cost) const {
        // Check funds
        if (current_funds < cost) {
            return false;
        }

        // Check space availability
        if (!grid_.IsSpaceAvailable(floor, column, width)) {
            return false;
        }

        // Additional validation could go here (adjacency rules, etc.)

        return true;
    }

    bool PlacementSystem::PlaceFacility(int floor, int column, const int facility_type_index, float &funds) {
        const auto &types = build_menu_.GetFacilityTypes();
        if (facility_type_index < 0 || facility_type_index >= static_cast<int>(types.size())) {
            return false;
        }

        const auto &facility_type = types[facility_type_index];

        // Calculate total cost including floor building
        const int floor_build_cost = facility_mgr_.CalculateFloorBuildCost(floor, column, facility_type.width);
        const int total_cost = facility_type.cost + floor_build_cost;

        // Validate placement
        if (!IsPlacementValid(floor, column, facility_type.width, funds, total_cost)) {
            // Provide specific feedback on why placement failed
            if (funds < total_cost) {
                // Insufficient funds feedback will be handled by game.cpp
            } else if (!grid_.IsSpaceAvailable(floor, column, facility_type.width)) {
                // Space not available feedback will be handled by game.cpp
            }
            return false;
        }

        // Build floors first (this doesn't use the command pattern as it's part of the facility placement)
        facility_mgr_.BuildFloorsForFacility(floor, column, facility_type.width);

        // Map to BuildingComponent::Type
        const auto bc_type = GetFacilityType(facility_type_index);

        // Create command
        auto command = std::make_unique<core::PlaceFacilityCommand>(
            facility_mgr_, grid_, bc_type, floor, column, facility_type.width, total_cost
        );

        // Execute via command history
        if (!command_history_.ExecuteCommand(std::move(command), funds)) {
            return false;
        }

        // Add to construction queue
        // Get the entity ID from the last undo entry
        if (const auto &undo_stack = command_history_.GetUndoStack(); !undo_stack.empty()) {
            const auto *place_cmd = dynamic_cast<core::PlaceFacilityCommand *>(
                undo_stack.back().command.get()
            );
            if (place_cmd) {
                float build_time = GetBuildTime(facility_type_index);
                constructions_in_progress_.emplace_back(
                    place_cmd->GetCreatedEntityId(), build_time, floor, column, facility_type.width
                );
            }
        }

        return true;
    }

    bool PlacementSystem::DemolishFacility(const int floor, const int column, float &funds) {
        if (!grid_.IsOccupied(floor, column)) {
            return false;
        }

        // Create demolish command
        auto command = std::make_unique<core::DemolishFacilityCommand>(
            facility_mgr_, grid_, floor, column, RECOVERY_PERCENTAGE
        );

        // Execute via command history
        return command_history_.ExecuteCommand(std::move(command), funds);
    }

    float PlacementSystem::GetBuildTime(const int facility_type_index) {
        // Build times in seconds (real-time)
        // Lobby: 10s, Office: 15s, Restaurant: 20s, Shop: 15s, Hotel: 25s
        // Gym: 18s, Arcade: 16s, Theater: 22s, Conference: 20s, Flagship: 28s, Elevator: 12s
        static constexpr float build_times[] = {
            10.0f, 15.0f, 20.0f, 15.0f, 25.0f, 18.0f, 16.0f, 22.0f, 20.0f, 28.0f, 12.0f
        };

        if (facility_type_index >= 0 && facility_type_index < 11) {
            return build_times[facility_type_index];
        }

        return 10.0f; // Default
    }

    core::BuildingComponent::Type PlacementSystem::GetFacilityType(const int facility_type_index) {
        switch (facility_type_index) {
            case 0: return core::BuildingComponent::Type::Lobby;
            case 1: return core::BuildingComponent::Type::Office;
            case 2: return core::BuildingComponent::Type::Restaurant;
            case 3: return core::BuildingComponent::Type::RetailShop;
            case 4: return core::BuildingComponent::Type::Hotel;
            case 5: return core::BuildingComponent::Type::Gym;
            case 6: return core::BuildingComponent::Type::Arcade;
            case 7: return core::BuildingComponent::Type::Theater;
            case 8: return core::BuildingComponent::Type::ConferenceHall;
            case 9: return core::BuildingComponent::Type::FlagshipStore;
            case 10: return core::BuildingComponent::Type::Elevator;
            default: return core::BuildingComponent::Type::Office;
        }
    }
}
