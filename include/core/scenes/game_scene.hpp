#pragma once

#include <functional>

namespace towerforge::core {

    class Game;

    class GameScene {
    public:
        explicit GameScene(Game* game) : game_(game) {}
        virtual ~GameScene() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual void Update(float delta_time) = 0;
        virtual void Render() = 0;

    protected:
        Game* game_;
    };

}
