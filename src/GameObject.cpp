#include <GameObject.h>
#include <Game.h>

std::vector<GameObject*> GameObject::gameObjects;

GameObject::GameObject( Game* game, const char* texPath,
                        b2Shape::Type type, b2Vec2 scale, b2Vec2 vertices[8])
{
    this->game = game;
    
    world = game->world;
    renderer = game->renderer;

    // TODO : keep textures with the same name in the memory
    // Do not load them over and over
    SDL_Surface* surface = IMG_Load(texPath);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    sizex = scale.x;
    sizey = scale.y;

    body = NULL;

    position = {0.0f, 0.0f};
    flip = SDL_FLIP_NONE;

    if (type != b2Shape::e_typeCount){
        createBody(scale, type, vertices);
        measureBody();
    }

    gameObjects.push_back(this);
}

GameObject::~GameObject(){
    for (int i = 0; i < GameObject::gameObjects.size(); i++){
        if (GameObject::gameObjects[i] == this){
            GameObject::gameObjects.erase(GameObject::gameObjects.begin() + i);
            break;
        }
    }

    world->DestroyBody(body);

    SDL_DestroyTexture(texture);
}

void GameObject::createBody(b2Vec2 scale, b2Shape::Type type, b2Vec2 vertices[8])
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(14, 7);
    body = world->CreateBody(&bodyDef);

    b2Shape* shape;

    if (type == b2Shape::e_circle){
        shape = new b2CircleShape();
        ((b2CircleShape*)shape)->m_radius = scale.x;
    }
    else if (type == b2Shape::e_polygon){
        shape = new b2PolygonShape();
        ((b2PolygonShape*)shape)->m_count = 4;
        for (int i = 0; i < 4; i++){
            vertices[i].x *= scale.x;
            vertices[i].y *= scale.y;
        }
        ((b2PolygonShape*)shape)->Set(vertices, 4);
    }
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.categoryBits = C1;
    fixtureDef.filter.maskBits = C1 | C2 | C3 | C4;
    body->CreateFixture(&fixtureDef);

    delete(shape);
}

void GameObject::measureBody(){
    if (body->GetFixtureList()->GetShape()->m_type == b2Shape::e_circle){
        sizex = ((b2CircleShape*)body->GetFixtureList()->GetShape())->m_radius;
        sizey = sizex;
    }
    else if (body->GetFixtureList()->GetShape()->m_type == b2Shape::e_polygon){
        sizex = 0.0f;
        sizey = 0.0f;
        b2PolygonShape* pShape = (b2PolygonShape*)body->GetFixtureList()->GetShape();
        for (int i = 0; i < pShape->m_count; i++){
            for (int j = 1; j < pShape->m_count; j++){
                float diffx = abs(pShape->m_vertices[i].x - pShape->m_vertices[j].x);
                float diffy = abs(pShape->m_vertices[i].y - pShape->m_vertices[j].y);
                if (diffx > sizex){
                    sizex = diffx;
                }
                if (diffy > sizey){
                    sizey = diffy;
                }
            }
        }
        sizex /= 2.0f;
        sizey /= 2.0f;
    }
}

void GameObject::render()
{
    SDL_Rect r;
    float a;
    if (body){
        r.x = body->GetPosition().x * 100 - sizex * 100;
        r.y = body->GetPosition().y * 100 - sizey * 100;
        a = body->GetAngle();
    }
    else{
        r.x = position.x * 100 - sizex * 100;
        r.y = position.y * 100 - sizey * 100;
        a = 0.0f;
    }
    r.w = sizex * 200;
    r.h = sizey * 200;

    SDL_RenderCopyEx(renderer, texture, NULL, &r, a * (180 / b2_pi), NULL, flip);
}