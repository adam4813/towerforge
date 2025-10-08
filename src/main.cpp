#include "core/game.h"

int main(int argc, char* argv[]) {
    towerforge::core::Game game;
    
    if (!game.Initialize()) {
        return 1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
}


