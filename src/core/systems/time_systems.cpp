#include "core/systems/time_systems.hpp"
#include "core/components.hpp"
#include <iostream>

namespace towerforge::core::Systems {

    void TimeSystems::RegisterAll(flecs::world& world) {
        RegisterTimeSimulation(world);
        RegisterScheduleExecution(world);
        RegisterTimeLogging(world);
    }

    void TimeSystems::RegisterTimeSimulation(flecs::world& world) {
        world.system<TimeManager>()
                .kind(flecs::PreUpdate)
                .each([](const flecs::entity e, TimeManager& time_mgr) {
                    const float delta_time = e.world().delta_time();
            
                    const float hours_elapsed = time_mgr.hours_per_second * time_mgr.simulation_speed * delta_time;
            
                    time_mgr.current_hour += hours_elapsed;
            
                    while (time_mgr.current_hour >= 24.0f) {
                        time_mgr.current_hour -= 24.0f;
                        time_mgr.current_day++;
                
                        if (time_mgr.current_day >= 7) {
                            time_mgr.current_day = 0;
                            time_mgr.current_week++;
                        }
                    }
                });
    }

    void TimeSystems::RegisterScheduleExecution(flecs::world& world) {
        world.system<DailySchedule, const Actor>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, DailySchedule& schedule, const Actor& actor) {
                    const auto& time_mgr = e.world().get<TimeManager>();
            
                    const auto& active_schedule = schedule.GetActiveSchedule(time_mgr.IsWeekend());
            
                    for (const auto& action : active_schedule) {
                        bool should_trigger = false;
                
                        if (schedule.last_triggered_hour < 0.0f) {
                            should_trigger = (time_mgr.current_hour >= action.trigger_hour);
                        } else {
                            if (schedule.last_triggered_hour > time_mgr.current_hour) {
                                should_trigger = (time_mgr.current_hour >= action.trigger_hour);
                            } else if (schedule.last_triggered_hour < action.trigger_hour && 
                                       time_mgr.current_hour >= action.trigger_hour) {
                                should_trigger = true;
                            }
                        }
                
                        if (should_trigger) {
                            auto action_name = "Unknown";
                            switch (action.type) {
                                case ScheduledAction::Type::ArriveWork:
                                    action_name = "Arriving at work";
                                    break;
                                case ScheduledAction::Type::LeaveWork:
                                    action_name = "Leaving work";
                                    break;
                                case ScheduledAction::Type::LunchBreak:
                                    action_name = "Taking lunch break";
                                    break;
                                case ScheduledAction::Type::Idle:
                                    action_name = "Going idle";
                                    break;
                                case ScheduledAction::Type::Custom:
                                    action_name = "Custom action";
                                    break;
                            }
                    
                            std::cout << "  [" << time_mgr.GetTimeString() << "] "
                                    << actor.name << ": " << action_name << std::endl;
                        }
                    }
            
                    schedule.last_triggered_hour = time_mgr.current_hour;
                });
    }

    void TimeSystems::RegisterTimeLogging(flecs::world& world) {
        world.system<const TimeManager>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)
                .each([](flecs::entity e, const TimeManager& time_mgr) {
                    std::cout << "  === Simulation Time: " << time_mgr.GetTimeString() 
                            << " " << time_mgr.GetDayName() 
                            << ", Week " << time_mgr.current_week
                            << " (Speed: " << time_mgr.simulation_speed << "x) ===" << std::endl;
                });
    }

}
