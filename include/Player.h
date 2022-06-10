#include <GameObject.h>

class Game;

enum KeyIndices{
    in_UP,
    in_DOWN,
    in_LEFT,
    in_RIGHT,
    in_SPACE,
};

class Player : public GameObject
{
    friend class Game;

private:
    int* keys;
    GameObject* foot;
    b2RevoluteJoint* j;

    float lastySpeed = 0.0f;

    int playerNum;

    SDL_Scancode playerKeys[2][5] = {
        {SDL_SCANCODE_UP,   SDL_SCANCODE_DOWN,  SDL_SCANCODE_LEFT,  SDL_SCANCODE_RIGHT, SDL_SCANCODE_J},
        {SDL_SCANCODE_W,    SDL_SCANCODE_S,     SDL_SCANCODE_A,     SDL_SCANCODE_D,     SDL_SCANCODE_SPACE}
    };

    SDL_RendererFlip isFlipped[2] = {SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL};

    b2Vec2 footVertices[2][8] = 
    {
        {b2Vec2(-0.5f, +0.1f), b2Vec2(-0.5f, +0.35f), b2Vec2(+0.5f, +0.35f), b2Vec2(+0.5f, -0.35f)},
        {b2Vec2(+0.5f, +0.1f), b2Vec2(+0.5f, +0.35f), b2Vec2(-0.5f, +0.35f), b2Vec2(-0.5f, -0.35f)},
    };

    float footInc[2] = {-0.8f, +0.8f};

    float footLimits[2][2] = {{b2_pi / 4.0f, -b2_pi / 3.0f}, {b2_pi / 3.0f, -b2_pi / 4.0f}};
    float playerMultipliers[2] = {+1.0f, -1.0f};

    b2Vec2 startPositions[2] = {b2Vec2(22.0f, 10.0f), b2Vec2(6.0f, 10.0f)};

public:
    int score = 0;

    Player(b2World* world, SDL_Renderer* renderer, const char* texPath, int pNum);

    void setFoot();

    void setKeys(int* keys);

    void handle();
};