// TowerForge main entry point
//
// Contributors: When adding new features, create and add relevant screenshots to the screenshots/ folder whenever possible.
// Documentation should be written or updated in the form of a user manual, focusing on usage and gameplay, not as an implementation summary.

#include <raylib.h>

#include "core/game.h"

int main(int argc, char* argv[]) {
    towerforge::core::Game game;
    
    if (!game.Initialize()) {
        return 1;
    }

    SetExitKey(0);
    
    game.Run();
    game.Shutdown();
    
    return 0;
}
