#pragma once

#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Centralized UI theme system providing consistent colors, spacing, and sizing
     * 
     * This class provides a unified theme for all UI elements throughout the application.
     * It defines standard colors, spacing values, animation parameters, and sizing constants
     * to ensure visual consistency and ease of maintenance.
     */
    class UITheme {
    public:
        // ===== Color Palette =====
        
        // Primary colors
        static constexpr Color PRIMARY = Color{220, 180, 70, 255};        // Gold accent
        static constexpr Color PRIMARY_DARK = Color{160, 130, 40, 255};   // Darker gold
        static constexpr Color PRIMARY_LIGHT = Color{255, 215, 100, 255}; // Lighter gold
        
        // Background colors
        static constexpr Color BACKGROUND_DARK = Color{20, 20, 30, 255};    // Main background
        static constexpr Color BACKGROUND_PANEL = Color{30, 30, 40, 255};   // Panel background
        static constexpr Color BACKGROUND_MODAL = Color{25, 25, 35, 255};   // Modal background
        
        // Overlay and transparency
        static inline Color OVERLAY_DARK = ColorAlpha(BLACK, 0.7f);        // Modal overlay
        static inline Color OVERLAY_LIGHT = ColorAlpha(BLACK, 0.5f);       // Light overlay
        
        // Text colors
        static constexpr Color TEXT_PRIMARY = WHITE;                        // Primary text
        static constexpr Color TEXT_SECONDARY = Color{180, 180, 190, 255}; // Secondary text
        static constexpr Color TEXT_DISABLED = Color{100, 100, 110, 255};  // Disabled text
        static constexpr Color TEXT_ACCENT = Color{220, 180, 70, 255};     // Accent text (gold)
        
        // Border colors
        static constexpr Color BORDER_DEFAULT = Color{128, 128, 128, 255}; // Default border
        static constexpr Color BORDER_ACCENT = Color{220, 180, 70, 255};   // Accent border (gold)
        static constexpr Color BORDER_SUBTLE = Color{60, 60, 70, 255};     // Subtle border
        
        // Button colors
        static inline Color BUTTON_BACKGROUND = ColorAlpha(DARKGRAY, 0.3f);
        static inline Color BUTTON_HOVER = ColorAlpha(GRAY, 0.5f);
        static inline Color BUTTON_PRESSED = ColorAlpha(GRAY, 0.7f);
        static constexpr Color BUTTON_BORDER = Color{128, 128, 128, 255};
        static constexpr Color BUTTON_BORDER_ACCENT = Color{220, 180, 70, 255};
        
        // State colors
        static constexpr Color SUCCESS = Color{80, 200, 120, 255};         // Success/positive
        static constexpr Color WARNING = Color{255, 200, 0, 255};          // Warning
        static constexpr Color ERROR = Color{220, 50, 50, 255};            // Error/danger
        static constexpr Color INFO = Color{100, 150, 255, 255};           // Info
        
        // Decorative colors
        static constexpr Color DECORATIVE_WINDOW = Color{40, 60, 100, 255}; // Building windows
        static inline Color DECORATIVE_GRID = ColorAlpha(DARKGRAY, 0.1f);   // Grid pattern
        
        // ===== Spacing and Sizing =====
        
        // Padding values
        static constexpr int PADDING_TINY = 5;
        static constexpr int PADDING_SMALL = 10;
        static constexpr int PADDING_MEDIUM = 15;
        static constexpr int PADDING_LARGE = 20;
        static constexpr int PADDING_XLARGE = 30;
        
        // Margins
        static constexpr int MARGIN_SMALL = 10;
        static constexpr int MARGIN_MEDIUM = 20;
        static constexpr int MARGIN_LARGE = 40;
        
        // Border widths
        static constexpr float BORDER_THIN = 1.0f;
        static constexpr float BORDER_NORMAL = 2.0f;
        static constexpr float BORDER_THICK = 3.0f;
        
        // UI element sizes
        static constexpr int BUTTON_HEIGHT_SMALL = 30;
        static constexpr int BUTTON_HEIGHT_MEDIUM = 40;
        static constexpr int BUTTON_HEIGHT_LARGE = 50;
        
        static constexpr int BUTTON_WIDTH_SMALL = 80;
        static constexpr int BUTTON_WIDTH_MEDIUM = 120;
        static constexpr int BUTTON_WIDTH_LARGE = 180;
        
        static constexpr int TITLE_BAR_HEIGHT = 25;
        static constexpr int CLOSE_BUTTON_SIZE = 15;
        
        // Font sizes
        static constexpr int FONT_SIZE_SMALL = 14;
        static constexpr int FONT_SIZE_NORMAL = 16;
        static constexpr int FONT_SIZE_MEDIUM = 20;
        static constexpr int FONT_SIZE_LARGE = 24;
        static constexpr int FONT_SIZE_TITLE = 60;
        
        // ===== Animation Parameters =====
        
        // Animation speeds (multiplier)
        static constexpr float ANIMATION_SPEED_SLOW = 2.0f;
        static constexpr float ANIMATION_SPEED_NORMAL = 3.0f;
        static constexpr float ANIMATION_SPEED_FAST = 5.0f;
        
        // Animation durations (seconds)
        static constexpr float ANIMATION_DURATION_QUICK = 0.15f;
        static constexpr float ANIMATION_DURATION_NORMAL = 0.3f;
        static constexpr float ANIMATION_DURATION_SLOW = 0.5f;
        
        // Press animation scale
        static constexpr float PRESS_SCALE_FACTOR = 0.95f;  // Button scales to 95% when pressed
        
        // Notification timers
        static constexpr float NOTIFICATION_DURATION_SHORT = 3.0f;
        static constexpr float NOTIFICATION_DURATION_NORMAL = 5.0f;
        static constexpr float NOTIFICATION_DURATION_LONG = 8.0f;
        
        // ===== Responsive Layout Helpers =====
        
        /**
         * @brief Calculate center position for an element
         * @param container_size Size of the container
         * @param element_size Size of the element to center
         * @return Position to center the element
         */
        static int CenterPosition(int container_size, int element_size) {
            return (container_size - element_size) / 2;
        }
        
        /**
         * @brief Calculate responsive width as percentage of screen width
         * @param percentage Percentage of screen width (0.0 - 1.0)
         * @return Width in pixels
         */
        static int ResponsiveWidth(float percentage) {
            return static_cast<int>(GetScreenWidth() * percentage);
        }
        
        /**
         * @brief Calculate responsive height as percentage of screen height
         * @param percentage Percentage of screen height (0.0 - 1.0)
         * @return Height in pixels
         */
        static int ResponsiveHeight(float percentage) {
            return static_cast<int>(GetScreenHeight() * percentage);
        }
        
        /**
         * @brief Clamp a value between min and max for responsive sizing
         * @param value Value to clamp
         * @param min_value Minimum value
         * @param max_value Maximum value
         * @return Clamped value
         */
        static int ClampSize(int value, int min_value, int max_value) {
            if (value < min_value) return min_value;
            if (value > max_value) return max_value;
            return value;
        }
        
        /**
         * @brief Calculate a responsive font size based on screen dimensions
         * @param base_size Base font size
         * @return Scaled font size
         */
        static int ResponsiveFontSize(int base_size) {
            // Scale font size based on screen width, with limits
            const int screen_width = GetScreenWidth();
            const float scale = screen_width / 1280.0f; // Base resolution: 1280px
            const int scaled = static_cast<int>(base_size * scale);
            return ClampSize(scaled, base_size - 4, base_size + 4);
        }
    };

}
