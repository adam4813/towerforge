#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Structure to hold income breakdown by facility type
     */
    struct IncomeBreakdown {
        struct FacilityTypeRevenue {
            std::string facility_type;
            float hourly_revenue;
            int facility_count;
            int total_tenants;
            float average_occupancy;
        };

        std::vector<FacilityTypeRevenue> revenues;
        float total_hourly_revenue = 0.0f;
        float total_operating_costs = 0.0f;
        float net_hourly_profit = 0.0f;
    };

    /**
     * @brief Overlay for displaying income analytics using citrus engine UI
     *
     * Standalone overlay class (not UIWindow-based).
     * Displays:
     * - Revenue summary (total, costs, net profit)
     * - Breakdown by facility type with tenant/occupancy info
     */
    class IncomeAnalyticsOverlay {
    public:
        using CloseCallback = std::function<void()>;

        explicit IncomeAnalyticsOverlay(const IncomeBreakdown &data);

        ~IncomeAnalyticsOverlay();

        /**
         * @brief Initialize UI components
         */
        void Initialize();

        /**
         * @brief Update overlay with new data
         */
        void Update(const IncomeBreakdown &data);

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
        void Show(const IncomeBreakdown &data);

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

        static constexpr int OVERLAY_WIDTH = 380;
        static constexpr int OVERLAY_HEIGHT = 450;

        std::string title_;
        bool visible_;
        IncomeBreakdown data_;
        CloseCallback close_callback_;

        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;

        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Container *content_container_;
    };
}
