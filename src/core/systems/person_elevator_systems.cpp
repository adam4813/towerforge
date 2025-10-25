#include "core/systems/person_elevator_systems.hpp"
#include "core/components.hpp"
#include <iostream>
#include <algorithm>

namespace TowerForge::Core::Systems {

    void PersonElevatorSystems::RegisterAll(flecs::world& world) {
        RegisterPersonHorizontalMovement(world);
        RegisterPersonWaiting(world);
        RegisterPersonElevatorRiding(world);
        RegisterPersonStateLogging(world);
        RegisterElevatorCarMovement(world);
        RegisterElevatorCall(world);
        RegisterPersonElevatorBoarding(world);
        RegisterElevatorLogging(world);
    }

    void PersonElevatorSystems::RegisterPersonHorizontalMovement(flecs::world& world) {
        world.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    const float delta_time = e.world().delta_time();
            
                    if (person.state == PersonState::Walking) {
                        const float direction = (person.destination_column > person.current_column) ? 1.0f : -1.0f;
                        const float distance_to_dest = std::abs(person.destination_column - person.current_column);
                
                        const float move_amount = person.move_speed * delta_time;
                
                        if (move_amount >= distance_to_dest) {
                            person.current_column = person.destination_column;
                    
                            if (person.HasReachedVerticalDestination()) {
                                person.state = PersonState::AtDestination;
                            } else {
                                person.state = PersonState::WaitingForElevator;
                            }
                        } else {
                            person.current_column += direction * move_amount;
                        }
                    }
                });
    }

    void PersonElevatorSystems::RegisterPersonWaiting(flecs::world& world) {
        world.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    if (person.state == PersonState::WaitingForElevator && 
                        !e.has<PersonElevatorRequest>()) {
                
                        int shaft_id = -1;
                        int shaft_column = -1;
                
                        e.world().each<ElevatorShaft>([&](const flecs::entity shaft_entity, const ElevatorShaft& shaft) {
                            if (shaft_id == -1 && shaft.ServesFloor(person.current_floor) && 
                                shaft.ServesFloor(person.destination_floor)) {
                                shaft_id = static_cast<int>(shaft_entity.id());
                                shaft_column = shaft.column;
                            }
                        });
                
                        if (shaft_id != -1) {
                            e.set<PersonElevatorRequest>({
                                shaft_id,
                                person.current_floor,
                                person.destination_floor
                            });
                    
                            if (std::abs(person.current_column - static_cast<float>(shaft_column)) > 0.1f) {
                                person.destination_column = static_cast<float>(shaft_column);
                                person.state = PersonState::Walking;
                            }
                        } else {
                            person.wait_time += e.world().delta_time();
                            if (person.wait_time > 5.0f) {
                                person.state = PersonState::InElevator;
                                person.wait_time = 0.0f;
                            }
                        }
                    }
                });
    }

    void PersonElevatorSystems::RegisterPersonElevatorRiding(flecs::world& world) {
        world.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    if (person.state == PersonState::InElevator && !e.has<PersonElevatorRequest>()) {
                        const float delta_time = e.world().delta_time();
                        person.wait_time += delta_time;
                
                        if (person.wait_time > 3.0f) {
                            person.current_floor = person.destination_floor;
                            person.wait_time = 0.0f;
                    
                            if (!person.HasReachedHorizontalDestination()) {
                                person.state = PersonState::Walking;
                            } else {
                                person.state = PersonState::AtDestination;
                            }
                        }
                    }
                });
    }

    void PersonElevatorSystems::RegisterPersonStateLogging(flecs::world& world) {
        world.system<const Person>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](flecs::entity e, const Person& person) {
                    std::cout << "  [Person] " << person.name 
                            << " - State: " << person.GetStateString()
                            << ", Floor: " << person.current_floor << " (" << person.current_column << ")"
                            << ", Dest: Floor " << person.destination_floor << " (" << person.destination_column << ")"
                            << ", Need: " << person.current_need;
            
                    if (person.state == PersonState::WaitingForElevator || person.state == PersonState::InElevator) {
                        std::cout << ", Wait: " << static_cast<int>(person.wait_time) << "s";
                    }
            
                    std::cout << std::endl;
                });
    }

    void PersonElevatorSystems::RegisterElevatorCarMovement(flecs::world& world) {
        world.system<ElevatorCar>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, ElevatorCar& car) {
                    const float delta_time = e.world().delta_time();
            
                    car.state_timer += delta_time;
            
                    switch (car.state) {
                        case ElevatorState::Idle:
                            if (!car.stop_queue.empty()) {
                                car.target_floor = car.GetNextStop();
                                const int current_floor = car.GetCurrentFloorInt();
                        
                                if (car.target_floor > current_floor) {
                                    car.state = ElevatorState::MovingUp;
                                } else if (car.target_floor < current_floor) {
                                    car.state = ElevatorState::MovingDown;
                                } else {
                                    car.state = ElevatorState::DoorsOpening;
                                    car.state_timer = 0.0f;
                                }
                            }
                            break;
                
                        case ElevatorState::MovingUp:
                        case ElevatorState::MovingDown: {
                            const float direction = (car.state == ElevatorState::MovingUp) ? 1.0f : -1.0f;
                            const float distance_to_target = std::abs(car.target_floor - car.current_floor);
                            const float move_amount = car.floors_per_second * delta_time;
                    
                            if (move_amount >= distance_to_target) {
                                car.current_floor = static_cast<float>(car.target_floor);
                                car.state = ElevatorState::DoorsOpening;
                                car.state_timer = 0.0f;
                            } else {
                                car.current_floor += direction * move_amount;
                            }
                            break;
                        }
                
                        case ElevatorState::DoorsOpening:
                            if (car.state_timer >= car.door_transition_duration) {
                                car.state = ElevatorState::DoorsOpen;
                                car.state_timer = 0.0f;
                                car.RemoveCurrentStop();
                            }
                            break;
                
                        case ElevatorState::DoorsOpen:
                            if (car.state_timer >= car.door_open_duration) {
                                car.state = ElevatorState::DoorsClosing;
                                car.state_timer = 0.0f;
                            }
                            break;
                
                        case ElevatorState::DoorsClosing:
                            if (car.state_timer >= car.door_transition_duration) {
                                car.state = ElevatorState::Idle;
                                car.state_timer = 0.0f;
                            }
                            break;
                    }
                });
    }

    void PersonElevatorSystems::RegisterElevatorCall(flecs::world& world) {
        world.system<Person, PersonElevatorRequest>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity person_entity, const Person& person, PersonElevatorRequest& request) {
                    const float delta_time = person_entity.world().delta_time();
            
                    request.wait_time += delta_time;
            
                    if (request.car_entity_id == -1 && person.state == PersonState::WaitingForElevator) {
                        const auto shaft_entity = person_entity.world().entity(request.shaft_entity_id);
                        if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                            person_entity.world().each<ElevatorCar>([&](const flecs::entity car_entity, ElevatorCar& car) {
                                if (car.shaft_entity_id == request.shaft_entity_id && 
                                    request.car_entity_id == -1) {
                                    request.car_entity_id = static_cast<int>(car_entity.id());
                                    car.AddStop(request.call_floor);
                                    car.AddStop(request.destination_floor);
                                }
                            });
                        }
                    }
                });
    }

    void PersonElevatorSystems::RegisterPersonElevatorBoarding(flecs::world& world) {
        world.system<Person, PersonElevatorRequest>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity person_entity, Person& person, PersonElevatorRequest& request) {
                    if (request.car_entity_id == -1) return;
            
                    const auto car_entity = person_entity.world().entity(request.car_entity_id);
                    if (!car_entity.is_valid() || !car_entity.has<ElevatorCar>()) return;
            
                    ElevatorCar& car = car_entity.ensure<ElevatorCar>();
            
                    if (person.state == PersonState::WaitingForElevator && 
                        car.state == ElevatorState::DoorsOpen &&
                        car.GetCurrentFloorInt() == request.call_floor &&
                        car.HasCapacity()) {
                        person.state = PersonState::InElevator;
                        person.current_floor = car.GetCurrentFloorInt();
                        person.wait_time = 0.0f;
                        car.current_occupancy++;
                        car.passenger_destinations.push_back(request.destination_floor);
                        request.is_boarding = false;
                    }
            
                    if (person.state == PersonState::InElevator &&
                        car.state == ElevatorState::DoorsOpen &&
                        car.GetCurrentFloorInt() == request.destination_floor) {
                        person.current_floor = car.GetCurrentFloorInt();
                        car.current_occupancy--;
                
                        const auto it = std::find(car.passenger_destinations.begin(), 
                                            car.passenger_destinations.end(), 
                                            request.destination_floor);
                        if (it != car.passenger_destinations.end()) {
                            car.passenger_destinations.erase(it);
                        }
                
                        person_entity.remove<PersonElevatorRequest>();
                
                        if (!person.HasReachedHorizontalDestination()) {
                            person.state = PersonState::Walking;
                        } else {
                            person.state = PersonState::AtDestination;
                        }
                    }
                });
    }

    void PersonElevatorSystems::RegisterElevatorLogging(flecs::world& world) {
        world.system<const ElevatorCar>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)
                .each([](const flecs::entity e, const ElevatorCar& car) {
                    std::cout << "  [Elevator] Car " << e.name().c_str()
                            << " - State: " << car.GetStateString()
                            << ", Floor: " << car.current_floor
                            << ", Occupancy: " << car.current_occupancy << "/" << car.max_capacity;
            
                    if (!car.stop_queue.empty()) {
                        std::cout << ", Stops: [";
                        for (size_t i = 0; i < car.stop_queue.size(); i++) {
                            std::cout << car.stop_queue[i];
                            if (i < car.stop_queue.size() - 1) std::cout << ", ";
                        }
                        std::cout << "]";
                    }
            
                    std::cout << std::endl;
                });
    }

}
