#include <box2d/box2d.h>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

enum entityCategory {
    C1 = 0x1 << 0,
    C2 = 0x1 << 1,
    C3 = 0x1 << 2,
    C4 = 0x1 << 3,
};

class GameObject{

public:
    static std::vector<GameObject*> gameObjects;

    b2World* world;
    b2Body* body;

    b2Vec2 position;

    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_RendererFlip flip;

    float sizex;
    float sizey;

    GameObject( b2World* world, SDL_Renderer* renderer, const char* texPath,
                b2Shape::Type type=b2Shape::e_circle, b2Vec2 scale=b2Vec2(1, 1), b2Vec2 vertices[8]={}, float density=1.0f);

    void createBody(b2Vec2 scale, b2Shape::Type type, b2Vec2 vertices[8], float density);

    void measureBody();

    void render();
};
