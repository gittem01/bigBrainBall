#include "GameObject.h"
#include <stdio.h>
#include <iostream>
#include <Game.h>

int main(void)
{
    Game* game = new Game();

    for (int i = 0; i < 20; i++){
        PowerUp* powerUp = new PowerUp(game, "../assets/powerOut.png");
        powerUp->body->SetTransform(b2Vec2(rand() % 28, rand() % 28), 0);
    }

    game->loop();
    SDL_Delay(100);
    game->lastTime = SDL_GetTicks();
    game->deltaTime = 0.0f;	

    while (1){
        game->loop();
	}

    return 0;
}
