#pragma once

#include <GameObject.h>

#define TRAIL_SIZE 200

class Ball : public GameObject
{

friend class Game;

private:
    SDL_Texture* trail;
    b2Vec2 ballTrail[TRAIL_SIZE];
    bool isGoal = false;
    float goalTimer = 0.0f;
    int ballTrailIndex = 0;
    long lastTrailTime = 0;

public:

    Ball(Game* game);

    int checkBallGoal();

    void trailRender();
    void check();

    void startThrow();
};