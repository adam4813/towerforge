#include "core/scenes/tutorial_scene.hpp"

#include <iostream>

#include "core/game.h"
#include "ui/tutorial_manager.h"

namespace towerforge::core {
	TutorialScene::TutorialScene(Game *game)
		: InGameScene(game)
		  , tutorial_active_(false) {
	}

	TutorialScene::~TutorialScene() = default;

	void TutorialScene::Initialize() {
		// Initialize game systems (from parent)
		InGameScene::Initialize();

		// Initialize tutorial manager
		tutorial_manager_ = std::make_unique<ui::TutorialManager>();
		tutorial_manager_->Initialize();
		tutorial_active_ = true;
		hud_->AddNotification(ui::Notification::Type::Info, "Welcome to the tutorial!", 5.0f);

		// Set build menu to tutorial mode if it exists
		// Will be accessed through parent's members once they're moved
	}

	void TutorialScene::Shutdown() {
		tutorial_manager_.reset();
		tutorial_active_ = false;

		// Shutdown game systems (from parent)
		InGameScene::Shutdown();

		tutorial_manager_.reset();
	}

	void TutorialScene::Update(const float delta_time) {
		// Update tutorial manager
		if (tutorial_manager_) {
			tutorial_manager_->Update(delta_time);
		}

		// Update game (from parent)
		InGameScene::Update(delta_time);

		// Handle tutorial-specific input
		HandleTutorialInput();
	}

	void TutorialScene::Render() {
		// Render game (from parent)
		InGameScene::Render();

		// Render tutorial overlay on top
		if (tutorial_manager_) {
			tutorial_manager_->Render();
		}
	}

	void TutorialScene::HandleTutorialInput() {
		if (!tutorial_manager_) {
			return;
		}

		// Check if tutorial should be skipped/exited
		if (tutorial_manager_->HandleInput()) {
			// Tutorial skipped - transition back to normal game
			tutorial_active_ = false;
			game_->SetGameState(GameState::InGame);
			return;
		}

		// Check if tutorial is complete
		if (tutorial_manager_->IsComplete()) {
			tutorial_active_ = false;
			game_->SetGameState(GameState::InGame);
		}
	}

	void TutorialScene::CreateStarterTower() const {
		if (!ecs_world_) return;

		auto &grid = ecs_world_->GetTowerGrid();
		auto &facility_mgr = ecs_world_->GetFacilityManager();

		std::cout << "Creating starter tower setup..." << std::endl;

		// Ensure we have enough floors for the starter tower
		// We need floors 0-2 (ground, 1st, 2nd)
		const int current_floors = grid.GetFloorCount();
		if (current_floors < 3) {
			const int floors_to_add = 3 - current_floors;
			grid.AddFloors(floors_to_add);
			std::cout << "  Added " << floors_to_add << " floors to tower" << std::endl;
		}

		// Build the starter tower as specified:
		// Floor 0: Lobby (already built by default)
		// Floor 1: Business (Office)
		// Floor 1 (different position): Shop (RetailShop)
		// Floors 0-2: Stair (not implemented as separate type yet)
		// Floor 2: Condo (Residential)
		// Floors 0-2: Elevator

		try {
			// Place Lobby at floor 0, column 0
			const auto lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0);
			if (lobby) {
				std::cout << "  Placed Lobby at (0,0)" << std::endl;
			}

			// Place Office at floor 1, column 0
			const auto office = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 0);
			if (office) {
				std::cout << "  Placed Office at (1,0)" << std::endl;
			}

			// Place RetailShop at floor 1, column 5
			const auto retail = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 1, 5);
			if (retail) {
				std::cout << "  Placed Retail at (1,5)" << std::endl;
			}

			// Place Residential at floor 2, column 0
			const auto residential = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 0);
			if (residential) {
				std::cout << "  Placed Residential at (2,0)" << std::endl;
			}

			// Place Elevator at floor 0, column 8
			const auto elevator = facility_mgr.CreateFacility(BuildingComponent::Type::Elevator, 0, 8);
			if (elevator) {
				std::cout << "  Placed Elevator at (0,8)" << std::endl;
			}

			std::cout << "Starter tower created successfully" << std::endl;
			hud_->AddNotification(ui::Notification::Type::Info, "Starter tower ready!", 5.0f);

			// Create initial staff for the tower
			std::cout << "Hiring initial staff..." << std::endl;

			// Hire a janitor for general cleaning (tower-wide)
			const auto janitor = ecs_world_->CreateEntity("Bob the Janitor");
			janitor.set<Person>({"Bob", 0, 3.0f, 2.0f, NPCType::Employee});
			janitor.set<StaffAssignment>({StaffRole::Janitor, -1, 6.0f, 18.0f});
			std::cout << "  Hired janitor: Bob (6:00 AM - 6:00 PM, tower-wide)" << std::endl;

			// Hire a maintenance technician (tower-wide)
			const auto maintenance = ecs_world_->CreateEntity("Carlos the Maintenance Tech");
			maintenance.set<Person>({"Carlos", 0, 4.0f, 2.0f, NPCType::Employee});
			maintenance.set<StaffAssignment>({StaffRole::Maintenance, -1, 8.0f, 17.0f});
			std::cout << "  Hired maintenance tech: Carlos (8:00 AM - 5:00 PM, tower-wide)" << std::endl;

			// Hire a firefighter (tower-wide, 24-hour shift)
			const auto firefighter = ecs_world_->CreateEntity("Dana the Firefighter");
			firefighter.set<Person>({"Dana", 0, 2.0f, 2.0f, NPCType::Employee});
			firefighter.set<StaffAssignment>({StaffRole::Firefighter, -1, 0.0f, 24.0f});
			std::cout << "  Hired firefighter: Dana (24/7, tower-wide)" << std::endl;

			// Add FacilityStatus to all facilities
			const auto &world = ecs_world_->GetWorld();
			world.defer([&] {
				world.each<BuildingComponent>([](const flecs::entity facility_entity, BuildingComponent &) {
					if (!facility_entity.has<FacilityStatus>()) {
						facility_entity.set<FacilityStatus>({});
					}
				});
			});

			std::cout << "Initial staff hired" << std::endl;
			hud_->AddNotification(ui::Notification::Type::Info, "Staff hired!", 3.0f);

			// Create staff from custom Lua roles (demo purposes - one of each type)
			const auto &custom_roles = ecs_world_->GetModManager().GetCustomStaffRoles();
			int custom_staff_count = 0;
			for (const auto &[role_id, role_data]: custom_roles) {
				// Create one staff member for each custom role
				const std::string staff_name = role_data.name + " #" + std::to_string(custom_staff_count + 1);
				const auto custom_staff = ecs_world_->CreateEntity(staff_name.c_str());
				custom_staff.set<Person>({staff_name, 0, 3.0f, 2.0f, NPCType::Employee});

				// Create custom staff assignment
				StaffAssignment assignment(StaffRole::Janitor, -1, role_data.shift_start_hour,
				                           role_data.shift_end_hour);
				assignment.custom_role_id = role_id;
				assignment.work_type = role_data.work_type;
				assignment.work_efficiency = role_data.work_efficiency;
				custom_staff.set<StaffAssignment>(assignment);

				std::cout << "  Hired custom staff: " << staff_name << " (" << role_data.name
						<< ", " << role_data.shift_start_hour << ":00 - " << role_data.shift_end_hour
						<< ":00, work type: " << role_data.work_type << ")" << std::endl;
				custom_staff_count++;
			}

			if (custom_staff_count > 0) {
				std::cout << "Hired " << custom_staff_count << " custom staff from mods" << std::endl;
			}
		} catch (const std::exception &e) {
			std::cerr << "Error creating starter tower: " << e.what() << std::endl;
		}
	}
}
