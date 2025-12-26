#pragma once

#include "hud/hud.h"
#include "ui/ui_theme.h"
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <functional>

import engine;

namespace towerforge::ui {

    /**
     * @brief Window for displaying facility information using citrus engine UI
     */
    class FacilityWindow {
    public:
        using CloseCallback = std::function<void()>;
        
        FacilityWindow(const FacilityInfo& info);
        
        void Render() const;
        void Update(float delta_time);
        void Update(const FacilityInfo& info);
        
        bool ProcessMouseEvent(const engine::ui::MouseEvent& event);
        
        void SetPosition(float x, float y);
        void SetCloseCallback(CloseCallback callback) { close_callback_ = callback; }
        
        void Show() { visible_ = true; }
        void Hide() { visible_ = false; }
        bool IsVisible() const { return visible_; }
        
        float GetWidth() const;
        float GetHeight() const;
    
    private:
        FacilityInfo info_;
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        bool visible_ = true;
        CloseCallback close_callback_;

        // Raw pointers to text elements for updates
        engine::ui::elements::Text* title_text_ = nullptr;
        engine::ui::elements::Button* close_button_ = nullptr;
        engine::ui::elements::Text* occupancy_value_ = nullptr;
        engine::ui::elements::Text* revenue_value_ = nullptr;
        engine::ui::elements::Text* satisfaction_value_ = nullptr;
        engine::ui::elements::Text* tenants_value_ = nullptr;
        engine::ui::elements::Text* cleanliness_state_value_ = nullptr;
        engine::ui::elements::Text* cleanliness_value_ = nullptr;
        engine::ui::elements::Text* maintenance_value_ = nullptr;
        engine::ui::elements::Text* maintenance_state_value_ = nullptr;
        engine::ui::elements::Container* adjacency_container_ = nullptr;
        engine::ui::elements::Button* demolish_button_ = nullptr;
        engine::ui::elements::Button* upgrade_button_ = nullptr;
        engine::ui::elements::Button* repair_button_ = nullptr;

        void BuildComponents();
        void UpdateComponentValues();
        static std::string GetSatisfactionEmoji(float satisfaction);
    };

    /**
     * @brief Window for displaying person information using citrus engine UI
     */
    class PersonWindow {
    public:
        using CloseCallback = std::function<void()>;
        
        PersonWindow(const PersonInfo& info);
        
        void Render() const;
        void Update(float delta_time);
        void Update(const PersonInfo& info);
        
        bool ProcessMouseEvent(const engine::ui::MouseEvent& event);
        
        void SetPosition(float x, float y);
        void SetCloseCallback(CloseCallback callback) { close_callback_ = callback; }
        
        void Show() { visible_ = true; }
        void Hide() { visible_ = false; }
        bool IsVisible() const { return visible_; }
        
        float GetWidth() const;
        float GetHeight() const;
    
    private:
        PersonInfo info_;
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        bool visible_ = true;
        CloseCallback close_callback_;

        // Raw pointers to text elements for updates
        engine::ui::elements::Text* title_text_ = nullptr;
        engine::ui::elements::Button* close_button_ = nullptr;
        engine::ui::elements::Text* type_value_ = nullptr;
        engine::ui::elements::Text* archetype_value_ = nullptr;
        engine::ui::elements::Text* role_value_ = nullptr;
        engine::ui::elements::Text* duty_value_ = nullptr;
        engine::ui::elements::Text* shift_value_ = nullptr;
        engine::ui::elements::Text* status_value_ = nullptr;
        engine::ui::elements::Text* state_value_ = nullptr;
        engine::ui::elements::Text* current_floor_value_ = nullptr;
        engine::ui::elements::Text* dest_floor_value_ = nullptr;
        engine::ui::elements::Text* wait_time_value_ = nullptr;
        engine::ui::elements::Text* hunger_value_ = nullptr;
        engine::ui::elements::Text* entertainment_value_ = nullptr;
        engine::ui::elements::Text* comfort_value_ = nullptr;
        engine::ui::elements::Text* shopping_value_ = nullptr;
        engine::ui::elements::Text* satisfaction_value_ = nullptr;

        void BuildComponents();
        void UpdateComponentValues();
        static std::string GetSatisfactionEmoji(float satisfaction);
    };

    /**
     * @brief Window for displaying elevator information using citrus engine UI
     */
    class ElevatorWindow {
    public:
        using CloseCallback = std::function<void()>;
        
        ElevatorWindow(const ElevatorInfo& info);
        
        void Render() const;
        void Update(float delta_time);
        void Update(const ElevatorInfo& info);
        
        bool ProcessMouseEvent(const engine::ui::MouseEvent& event);
        
        void SetPosition(float x, float y);
        void SetCloseCallback(CloseCallback callback) { close_callback_ = callback; }
        
        void Show() { visible_ = true; }
        void Hide() { visible_ = false; }
        bool IsVisible() const { return visible_; }
        
        float GetWidth() const;
        float GetHeight() const;
    
    private:
        ElevatorInfo info_;
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        bool visible_ = true;
        CloseCallback close_callback_;

        // Raw pointers to text elements for updates
        engine::ui::elements::Text* title_text_ = nullptr;
        engine::ui::elements::Button* close_button_ = nullptr;
        engine::ui::elements::Text* current_floor_value_ = nullptr;
        engine::ui::elements::Text* occupancy_value_ = nullptr;
        engine::ui::elements::Text* next_stop_value_ = nullptr;
        engine::ui::elements::Text* queue_length_value_ = nullptr;
        engine::ui::elements::Container* queue_container_ = nullptr;

        void BuildComponents();
        void UpdateComponentValues();
    };

}
