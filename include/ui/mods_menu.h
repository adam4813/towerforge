#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge {
namespace ui {

class ModsMenu {
public:
    ModsMenu();
    ~ModsMenu();
    
    void Update(float delta_time);
    void Render();
    
    bool IsVisible() const { return visible_; }
    void Show() { visible_ = true; }
    void Hide() { visible_ = false; }
    
private:
    void RenderBackground();
    void RenderModList();
    void RenderModDetails();
    
    bool visible_;
    float animation_time_;
    int selected_mod_index_;
    
    struct ModInfo {
        std::string name;
        std::string author;
        std::string version;
        bool enabled;
    };
    
    std::vector<ModInfo> mods_;
};

} // namespace ui
} // namespace towerforge
