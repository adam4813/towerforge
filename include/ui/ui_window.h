#pragma once

#include <raylib.h>
#include <string>
#include <memory>
#include <functional>
#include "ui/ui_element.h"
#include "ui/window_chrome.h"

namespace towerforge::ui {

    /**
     * @brief Base class for all UI windows
     * 
     * Now inherits from Panel (Composite pattern), integrating into the UIElement hierarchy.
     * Uses WindowChrome (Strategy pattern) for rendering window decoration.
     * 
     * Benefits:
     * - Automatic event handling via ProcessMouseEvent
     * - Can add child UIElements with proper composition
     * - Eliminates parallel hierarchy with UIElement
     * - Consistent positioning, hit testing, rendering
     */
    class UIWindow : public Panel {
    public:
        using CloseCallback = std::function<void()>;

        /**
         * @brief Construct a UI window
         * @param title Window title text
         * @param width Window width
         * @param height Window height
         */
        UIWindow(const std::string& title, float width, float height);
        virtual ~UIWindow() = default;
    
        /**
         * @brief Render the window (chrome + children)
         */
        void Render() const override;
    
        /**
         * @brief Update window (for animations, repositioning, etc.)
         */
        void Update(float delta_time) override;
    
        /**
         * @brief Process mouse events (handles close button + delegates to children)
         */
        bool ProcessMouseEvent(const MouseEvent& event) override;
    
        /**
         * @brief Get window ID (unique identifier)
         */
        int GetId() const { return id_; }
        
        /**
         * @brief Set window title
         */
        void SetTitle(const std::string& title) { title_ = title; }
        
        /**
         * @brief Get window title
         */
        const std::string& GetTitle() const { return title_; }
        
        /**
         * @brief Set close callback (called when X button clicked)
         */
        void SetCloseCallback(CloseCallback callback) { close_callback_ = callback; }
        
        /**
         * @brief Set window position (overrides Panel to update both)
         */
        void SetWindowPosition(float x, float y);
        
        /**
         * @brief Get z-order (higher values are rendered on top)
         */
        int GetZOrder() const { return z_order_; }
    
        /**
         * @brief Set z-order
         */
        void SetZOrder(int z_order) { z_order_ = z_order; }
    
    protected:
        /**
         * @brief Render window content (override in subclasses)
         * Default implementation renders all children
         */
        virtual void RenderContent() const;
        
        int id_;
        std::string title_;
        int z_order_;
        CloseCallback close_callback_;
        
        // Strategy pattern: delegate chrome rendering
        WindowChrome chrome_;
    
    private:
        static int next_id_;
    };

}
