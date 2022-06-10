#include <stdio.h>
#include <iostream>
#include <Game.h>

int main(void)
{
    Game* game = new Game();
    game->loop();
    SDL_Delay(100);
    game->lastTime = SDL_GetTicks();
    game->deltaTime = 0.0f;
    while (1){
        game->loop();
    }

    return 0;
}
