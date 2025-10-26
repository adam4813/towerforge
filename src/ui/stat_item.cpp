#include "ui/stat_item.h"

namespace towerforge::ui {

    StatItem::StatItem(const float relative_x, const float relative_y, const std::string& label)
        : UIElement(relative_x, relative_y, 200, 20) {
        
        // Create label (left side)
        auto label_elem = std::make_unique<Label>(0, 0, label, 14, LIGHTGRAY, Label::Alignment::Left);
        label_ = label_elem.get();
        AddChild(std::move(label_elem));
        
        // Create value (right side)
        auto value_elem = std::make_unique<Label>(100, 0, "", 14, LIGHTGRAY, Label::Alignment::Left);
        value_ = value_elem.get();
        AddChild(std::move(value_elem));
    }

    void StatItem::SetValue(const std::string& value, const Color color) {
        value_->SetText(value);
        value_->SetColor(color);
    }
    
    void StatItem::Render() const {
        // Render both label children
        for (const auto& child : children_) {
            child->Render();
        }
    }

}
