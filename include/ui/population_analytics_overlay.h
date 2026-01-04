#pragma once

#include <memory>
#include <string>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Structure to hold population breakdown
     */
    struct PopulationBreakdown {
        int total_population;
        int employees;
        int visitors;
        int residential_occupancy;
        int residential_capacity;
        float average_satisfaction;
    };

    /**
     * @brief Overlay for displaying population analytics using citrus engine UI
     *
     * Standalone overlay class (not UIWindow-based).
     * Displays:
     * - Population breakdown (total, employees, visitors)
     * - Residential occupancy and capacity
     * - Average satisfaction
     */
    class PopulationAnalyticsOverlay {
    public:
        using CloseCallback = std::function<void()>;

        explicit PopulationAnalyticsOverlay(const PopulationBreakdown &data);

        ~PopulationAnalyticsOverlay();

        /**
         * @brief Initialize UI components
         */
        void Initialize();

        /**
         * @brief Update overlay with new data
         */
        void Update(const PopulationBreakdown &data);

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
        void Show(const PopulationBreakdown &data);

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

        static constexpr int OVERLAY_WIDTH = 350;
        static constexpr int OVERLAY_HEIGHT = 350;

        std::string title_;
        bool visible_;
        PopulationBreakdown data_;
        CloseCallback close_callback_;

        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;

        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Container *content_container_;
    };
}
