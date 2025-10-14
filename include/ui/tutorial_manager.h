#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge::ui {

    /**
 * @brief Tutorial step enumeration
 */
    enum class TutorialStep {
        BuildLobby = 0,
        BuildBusiness,
        BuildShop,
        BuildStair,
        BuildCondo,
        BuildElevator,
        Complete
    };

    /**
 * @brief Tutorial manager for guiding new players through basic construction
 */
    class TutorialManager {
    public:
        TutorialManager();
        ~TutorialManager();
    
        /**
     * @brief Initialize or reset tutorial
     */
        void Initialize();
    
        /**
     * @brief Update tutorial state
     * @param delta_time Time elapsed since last frame
     */
        void Update(float delta_time);
    
        /**
     * @brief Render tutorial overlay
     */
        void Render();
    
        /**
     * @brief Handle keyboard/mouse input
     * @return True if tutorial should be skipped/exited
     */
        bool HandleInput();
    
        /**
     * @brief Notify tutorial of facility placement
     * @param facility_type Type of facility placed
     */
        void OnFacilityPlaced(const std::string& facility_type);
    
        /**
     * @brief Check if tutorial is complete
     */
        bool IsComplete() const { return current_step_ == TutorialStep::Complete; }
    
        /**
     * @brief Get current tutorial step
     */
        TutorialStep GetCurrentStep() const { return current_step_; }
    
        /**
     * @brief Get which facility type should be highlighted (for build menu)
     * @return Facility name to highlight, or empty string if none
     */
        std::string GetHighlightedFacility() const;
    
        /**
     * @brief Check if a facility type is allowed to be built
     * @param facility_type Type of facility to check
     * @return True if allowed
     */
        bool IsFacilityAllowed(const std::string& facility_type) const;
    
    private:
        void RenderOverlay() const;
        void RenderStepInfo() const;
        void RenderProgress();

        static void RenderButtons();
    
        std::string GetStepTitle() const;
        std::string GetStepHint() const;
        std::string GetRequiredFacility() const;
    
        void AdvanceToNextStep();
    
        TutorialStep current_step_;
        float animation_time_;
        bool skip_requested_;
    
        // Tutorial progress tracking
        bool lobby_built_;
        bool business_built_;
        bool shop_built_;
        bool stair_built_;
        bool condo_built_;
        bool elevator_built_;
    
        // UI layout constants
        static constexpr int OVERLAY_WIDTH = 600;
        static constexpr int OVERLAY_HEIGHT = 200;
        static constexpr int BUTTON_WIDTH = 150;
        static constexpr int BUTTON_HEIGHT = 40;
    };

}
