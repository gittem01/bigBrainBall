#pragma once

#include <GameObject.h>

#define TRAIL_SIZE 1000

class Ball : public GameObject
{

friend class Game;

private:
    SDL_Texture* trail;
    b2Vec2 ballTrail[TRAIL_SIZE];
    bool isGoal = false;
    float goalTimer = 0.0f;
    int ballTrailIndex = 0;
    float lastTrailTime = 0.0f;
    float everyT = 0.0001f;

public:

    Ball(Game* game);

    int checkBallGoal();

    void trailRender();
    void check(float dt);

    void startThrow();
};