#include "ui/placement_system.h"
#include "rendering/camera.h"
#include "core/command.hpp"
#include <iostream>
#include <sstream>

#include "ui/tooltip.h"

namespace towerforge::ui {
    PlacementSystem::PlacementSystem(core::TowerGrid &grid, core::FacilityManager &facility_mgr, BuildMenu &build_menu)
        : grid_(grid)
          , facility_mgr_(facility_mgr)
          , build_menu_(build_menu)
          , camera_(nullptr)
          , demolish_mode_(false)
          , hover_floor_(-1)
          , hover_column_(-1)
          , hover_valid_(false)
          , command_history_(50) // Max 50 actions in history
          , tooltip_manager_(nullptr)
          , pending_demolish_floor_(-1)
          , pending_demolish_column_(-1)
          , pending_demolish_funds_(0.0f)
          , pending_funds_change_(0) {
        // Create demolish confirmation dialog
        demolish_confirmation_ = std::make_unique<EngineConfirmationDialog>(
            "Confirm Demolish",
            "Are you sure you want to demolish this facility? You will receive 50% of the original cost as a refund.",
            "Demolish",
            "Cancel"
        );
        demolish_confirmation_->Initialize();

        // Set up the confirmation callback to actually perform demolish
        demolish_confirmation_->SetConfirmCallback([this]() {
            if (pending_demolish_floor_ >= 0 && pending_demolish_column_ >= 0) {
                float funds = pending_demolish_funds_;
                const float funds_before = funds;
                if (DemolishFacility(pending_demolish_floor_, pending_demolish_column_, funds)) {
                    pending_funds_change_ = static_cast<int>(funds - funds_before);
                }
                pending_demolish_floor_ = -1;
                pending_demolish_column_ = -1;
            }
        });
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

        // Update confirmation dialog
        if (demolish_confirmation_) {
            demolish_confirmation_->Update(delta_time);
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

            // Determine hover validity
            if (demolish_mode_) {
                // Valid if there's a facility to demolish
                hover_valid_ = grid_.IsOccupied(floor, column);
            } else {
                const int selected = build_menu_.GetSelectedFacility();
                if (selected >= 0) {
                    const auto &types = build_menu_.GetFacilityTypes();
                    const auto &facility_type = types[selected];

                    // Check if placement is valid (will be checked with funds in HandleClick)
                    hover_valid_ = grid_.IsSpaceAvailable(floor, column, facility_type.width);
                } else {
                    hover_valid_ = false;
                }
            }
        } else {
            hover_floor_ = -1;
            hover_column_ = -1;
            hover_valid_ = false;
        }

        // Render placement preview
        if (hover_floor_ >= 0 && hover_column_ >= 0) {
            const int selected = build_menu_.GetSelectedFacility();

            if (demolish_mode_) {
                // Red highlight for demolish
                if (hover_valid_) {
                    const int facility_id = grid_.GetFacilityAt(hover_floor_, hover_column_);
                    if (facility_id >= 0) {
                        // Find the full width of the facility
                        int start_col = hover_column_;
                        while (start_col > 0 && grid_.GetFacilityAt(hover_floor_, start_col - 1) == facility_id) {
                            start_col--;
                        }
                        int end_col = hover_column_;
                        while (end_col < grid_.GetColumnCount() - 1 &&
                               grid_.GetFacilityAt(hover_floor_, end_col + 1) == facility_id) {
                            end_col++;
                        }
                        const int width = end_col - start_col + 1;

                        // Draw red outline around facility to demolish
                        const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
                        const int x = grid_offset_x + start_col * cell_width;
                        const int y = ground_floor_screen_y - (hover_floor_ * cell_height);
                        DrawRectangle(x, y, width * cell_width, cell_height, ColorAlpha(RED, 0.3f));
                        DrawRectangleLines(x, y, width * cell_width, cell_height, RED);
                    }
                }
            } else if (selected >= 0) {
                const auto &types = build_menu_.GetFacilityTypes();
                const auto &facility_type = types[selected];

                // Draw ghost preview
                const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
                const int x = grid_offset_x + hover_column_ * cell_width;
                const int y = ground_floor_screen_y - (hover_floor_ * cell_height);

                const Color preview_color = hover_valid_ ? ColorAlpha(GREEN, 0.3f) : ColorAlpha(RED, 0.3f);
                const Color outline_color = hover_valid_ ? GREEN : RED;

                DrawRectangle(x, y, facility_type.width * cell_width, cell_height, preview_color);
                DrawRectangleLines(x, y, facility_type.width * cell_width, cell_height, outline_color);

                // Draw icon
                DrawText(facility_type.icon.c_str(), x + 5, y + 5, 20, WHITE);

                // Draw cost indicator
                if (hover_valid_) {
                    DrawText(TextFormat("$%d", facility_type.cost), x + 5, y + cell_height - 20, 12, GREEN);
                } else {
                    DrawText("INVALID", x + 5, y + cell_height - 20, 12, RED);
                }
            }
        }

        // Render construction progress
        const int ground_floor_screen_y = grid_offset_y + (grid_.GetFloorCount() / 2) * cell_height;
        for (const auto &construction: constructions_in_progress_) {
            const int x = grid_offset_x + construction.column * cell_width;
            const int y = ground_floor_screen_y - (construction.floor * cell_height);

            // Draw construction overlay
            DrawRectangle(x, y, construction.width * cell_width, cell_height,
                          ColorAlpha(ORANGE, 0.4f));

            // Draw progress bar
            const int bar_width = construction.width * cell_width - 10;
            constexpr int bar_height = 8;
            const int bar_x = x + 5;
            const int bar_y = y + cell_height / 2 - bar_height / 2;

            DrawRectangle(bar_x, bar_y, bar_width, bar_height, DARKGRAY);
            DrawRectangle(bar_x, bar_y, static_cast<int>(bar_width * construction.GetProgress()),
                          bar_height, YELLOW);

            // Draw construction text
            DrawText(TextFormat("Building... %d%%", static_cast<int>(construction.GetProgress() * 100)),
                     x + 5, y + 5, 10, WHITE);
        }

        // Render confirmation dialog if visible
        if (demolish_confirmation_ && demolish_confirmation_->IsVisible()) {
            demolish_confirmation_->Render();
        }
    }

    void PlacementSystem::Render() const {
        // Render confirmation dialog if visible
        if (demolish_confirmation_ && demolish_confirmation_->IsVisible()) {
            demolish_confirmation_->Render();
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

        if (demolish_mode_) {
            // Show confirmation dialog for demolish
            if (grid_.IsOccupied(floor, column)) {
                pending_demolish_floor_ = floor;
                pending_demolish_column_ = column;
                pending_demolish_funds_ = current_funds;

                // Get facility info for dialog message
                const int facility_id = grid_.GetFacilityAt(floor, column);
                const auto facility_type = facility_mgr_.GetFacilityType(facility_id);
                const std::string facility_name = core::FacilityManager::GetTypeName(facility_type);

                demolish_confirmation_->Show();

                return 0; // Don't apply funds change yet, wait for confirmation
            }
        } else {
            if (const int selected = build_menu_.GetSelectedFacility(); selected >= 0) {
                if (float funds = current_funds; PlaceFacility(floor, column, selected, funds)) {
                    return static_cast<int>(funds - current_funds); // Negative (cost)
                }
            }
        }

        return 0;
    }

    bool PlacementSystem::ProcessMouseEvent(const MouseEvent &event) const {
        // If confirmation dialog is visible, route events to it first
        if (demolish_confirmation_ && demolish_confirmation_->IsVisible()) {
            const engine::ui::MouseEvent engine_event{
                event.x,
                event.y,
                event.left_down,
                event.right_down,
                event.left_pressed,
                event.right_pressed,
            };
            return demolish_confirmation_->ProcessMouseEvent(engine_event);
        }
        return false;
    }

    int PlacementSystem::GetPendingFundsChange() {
        const int change = pending_funds_change_;
        pending_funds_change_ = 0; // Reset after reading
        return change;
    }

    void PlacementSystem::ShowDemolishConfirmation(const int clicked_floor, const int clicked_column, const int cost) {
        demolish_confirmation_->Show();
        pending_demolish_floor_ = clicked_floor;
        pending_demolish_column_ = clicked_column;
        pending_demolish_funds_ = static_cast<float>(cost);
    }

    bool PlacementSystem::HandleKeyboard() {
        // Keyboard shortcuts
        // 1-5 for facility types
        if (IsKeyPressed(KEY_ONE)) {
            if (build_menu_.GetFacilityTypes().size() > 0) {
                demolish_mode_ = false;
                // Select index 0 (Lobby)
                return true;
            }
        } else if (IsKeyPressed(KEY_TWO)) {
            if (build_menu_.GetFacilityTypes().size() > 1) {
                demolish_mode_ = false;
                // Select index 1 (Office)
                return true;
            }
        } else if (IsKeyPressed(KEY_THREE)) {
            if (build_menu_.GetFacilityTypes().size() > 2) {
                demolish_mode_ = false;
                // Select index 2
                return true;
            }
        } else if (IsKeyPressed(KEY_FOUR)) {
            if (build_menu_.GetFacilityTypes().size() > 3) {
                demolish_mode_ = false;
                // Select index 3
                return true;
            }
        } else if (IsKeyPressed(KEY_FIVE)) {
            if (build_menu_.GetFacilityTypes().size() > 4) {
                demolish_mode_ = false;
                // Select index 4
                return true;
            }
        }

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

            if (demolish_mode_) {
                // Check if there's a facility to demolish
                if (const auto facility_id = grid_.GetFacilityAt(grid_y, grid_x); facility_id >= 0) {
                    const auto buildingType = facility_mgr_.GetFacilityType(facility_id);
                    const auto name = core::FacilityManager::GetTypeName(buildingType);
                    tooltip_text << "Demolish " << name << "\n";
                } else {
                    tooltip_text << "No facility to demolish";
                }
            } else {
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
        const auto &undo_stack = command_history_.GetUndoStack();
        if (!undo_stack.empty()) {
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
        // Map build menu index to BuildingComponent::Type
        // Based on BuildMenu initialization order:
        // 0: Lobby, 1: Office, 2: Restaurant, 3: Shop (RetailShop), 4: Hotel,
        // 5: Gym, 6: Arcade, 7: Theater, 8: Conference (ConferenceHall),
        // 9: Flagship (FlagshipStore), 10: Elevator

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
