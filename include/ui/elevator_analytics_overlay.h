#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Structure to hold elevator usage statistics
     */
    struct ElevatorAnalytics {
        struct ElevatorStats {
            int elevator_id;
            int total_trips;
            float average_wait_time;
            float utilization_rate; // Percentage of time elevator is occupied
            int total_passengers_carried;
            std::vector<std::pair<int, int> > floor_traffic; // floor, passenger count
        };

        std::vector<ElevatorStats> elevators;
        int total_passengers = 0;
        float average_wait_time = 0.0f;
        int busiest_floor = 0;
        int busiest_floor_count = 0;
    };

    /**
     * @brief Overlay for displaying elevator analytics using citrus engine UI
     *
     * Standalone overlay class (not UIWindow-based).
     * Displays:
     * - Overall statistics (total passengers, avg wait time, busiest floor)
     * - Per-elevator performance (trips, passengers, utilization, wait time)
     */
    class ElevatorAnalyticsOverlay {
    public:
        using CloseCallback = std::function<void()>;

        explicit ElevatorAnalyticsOverlay(const ElevatorAnalytics &data);

        ~ElevatorAnalyticsOverlay();

        /**
         * @brief Initialize UI components
         */
        void Initialize();

        /**
         * @brief Update overlay with new data
         */
        void Update(const ElevatorAnalytics &data);

        /**
         * @brief Update layout (handles window resize)
         */
        void UpdateLayout();

        /**
         * @brief Render the overlay
         */
        void Render() const;

        /**
         * @brief Process mouse events
         * @return true if event was handled
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Handle keyboard input
         */
        void HandleKeyboard();

        /**
         * @brief Set close callback
         */
        void SetCloseCallback(CloseCallback callback) { close_callback_ = std::move(callback); }

        /**
         * @brief Get overlay title
         */
        const std::string &GetTitle() const { return title_; }

        /**
         * @brief Check if overlay is visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Show the overlay
         */
        void Show();

        /**
         * @brief Show the overlay with new data
         */
        void Show(const ElevatorAnalytics &data);

        /**
         * @brief Hide the overlay
         */
        void Hide();

        /**
         * @brief Shutdown and cleanup resources
         */
        void Shutdown();

    private:
        void RebuildContent() const;

        static void RenderDimOverlay();

        static constexpr int OVERLAY_WIDTH = 400;
        static constexpr int OVERLAY_HEIGHT = 500;

        std::string title_;
        bool visible_;
        ElevatorAnalytics data_;
        CloseCallback close_callback_;

        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;

        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Container *content_container_;
    };
}
