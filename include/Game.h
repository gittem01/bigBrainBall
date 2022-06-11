#pragma once

#include <Player.h>
#include <Ball.h>
#include <PowerUp.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <semaphore>

#define MAX_NUM 1000

typedef struct Rect{
    float x;
    float y;
    float w;
    float h;
} Rect;

typedef struct CharTex{
    SDL_Surface* surface;
    SDL_Texture* texture;
} CharTex;

class Game
{
private:
    CharTex* dash;
    CharTex numbers[MAX_NUM];
    SDL_Texture* bg;

public:
    pthread_t thread;
    std::binary_semaphore bSem{1};
    std::binary_semaphore exSem{1};

    SDL_Window* window;
    SDL_Renderer* renderer;
    float dpiScaling;

    b2World* world;
    SDL_Event event;
    int* keys;

    std::vector<SDL_Joystick*> joysticks;

    Ball* ball;
    GameObject* goal1;
    GameObject* goal2;

    Player* player1;
    Player* player2;

    Rect goalRects[2];
    Rect topRects[2];

    float timeMult = 1.0f;

    long lastTimeP;
    long deltaTimeP;

    Game(){
        SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);

        window = SDL_CreateWindow("Big Brain Ball", 0, 0, 1423, 768, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

        int x1, y1, x2, y2;
        SDL_GetWindowSize(window, &x1, &y1);
        SDL_GL_GetDrawableSize(window, &x2, &y2);

        dpiScaling = x2 / (float)x1;

        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        for (int i = 0; i < SDL_NumJoysticks(); i++){
            SDL_Joystick* joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            printf("Opened %s\n", SDL_JoystickName(joystick));
        }

        if (TTF_Init() < 0) {
            std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
        }

        TTF_Font* font;

        font = TTF_OpenFont("../assets/josefinSans.ttf", 150);
        if (!font) {
            std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        }

        SDL_Color color = {255, 255, 255};

        for (int i = 0; i < MAX_NUM; i++){
            char num[7];
            SDL_itoa(i, num, 10);
            SDL_Surface* numSurface = TTF_RenderText_Solid(font, num, color);
            SDL_Texture* numTexture = SDL_CreateTextureFromSurface(renderer, numSurface);
            numbers[i].surface = numSurface;
            numbers[i].texture = numTexture;
        }

        dash = new CharTex();
        SDL_Surface* dashSurface = TTF_RenderText_Solid(font, "-", color);
        dash->surface = dashSurface;
        dash->texture = SDL_CreateTextureFromSurface(renderer, dashSurface);

        keys = (int*)calloc(SDL_NUM_SCANCODES, sizeof(int));

        goal1 = new GameObject(this, "../assets/goal.png", b2Shape::e_typeCount, b2Vec2(1.75f, 2.25f));
        GameObject::gameObjects.erase(GameObject::gameObjects.end() - 1);
        goal1->position.x = 28.46 - 1.5f;
        goal1->position.y = 12.1f;

        goal2 = new GameObject(this, "../assets/goal.png", b2Shape::e_typeCount, b2Vec2(1.75f, 2.25f));
        GameObject::gameObjects.erase(GameObject::gameObjects.end() - 1);
        goal2->flip = SDL_FLIP_HORIZONTAL;
        goal2->position.x = 1.5f;
        goal2->position.y = 12.1f;
        
        goalRects[0].x = 0.0f;
        goalRects[0].y = 10.0f;
        goalRects[0].w = 2.55f;
        goalRects[0].h = 4.36f;

        goalRects[1].x = 28.46f - 2.55f;
        goalRects[1].y = 10.0f;
        goalRects[1].w = 2.55f;
        goalRects[1].h = 4.36f;

        SDL_Surface* s = IMG_Load("../assets/field.png");
        bg = SDL_CreateTextureFromSurface(renderer, s);

        createWorld();

        player1 = new Player(this, "../assets/head.png", 0);
        player1->body->SetFixedRotation(true);
        player1->setKeys(keys);

        player2 = new Player(this, "../assets/head.png", 1);
        player2->body->SetFixedRotation(true);
        player2->setKeys(keys);

        ball = new Ball(this);

        topRects[0].x = 0.0f;
        topRects[0].y = 9.9f - ((b2CircleShape*)ball->body->GetFixtureList()->GetShape())->m_radius * 2;
        topRects[0].w = 3.0f;
        topRects[0].h = ((b2CircleShape*)ball->body->GetFixtureList()->GetShape())->m_radius * 2;

        topRects[1].x = 28.46f - 3.2f;
        topRects[1].y = 9.9f - ((b2CircleShape*)ball->body->GetFixtureList()->GetShape())->m_radius * 2;
        topRects[1].w = 3.2f;
        topRects[1].h = ((b2CircleShape*)ball->body->GetFixtureList()->GetShape())->m_radius * 2;

        pthread_create(&thread, NULL, threadFunc, (void*)this);
    }

    void createWorld(){
        world = new b2World(b2Vec2(0.0f, 30.0f));

        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;

        b2Body* body = world->CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0f;
        fixtureDef.filter.categoryBits = C2;
        fixtureDef.filter.maskBits = C1 | C2 | C3 | C4;
        b2EdgeShape edgeShape;
        edgeShape.m_vertex1.Set(0, 14.36);
        edgeShape.m_vertex2.Set(28.46, 14.36);
        fixtureDef.shape = &edgeShape;
        body->CreateFixture(&fixtureDef);

        edgeShape.m_vertex1.Set(0, 0);
        edgeShape.m_vertex2.Set(0, 15.36);
        body->CreateFixture(&fixtureDef);

        edgeShape.m_vertex1.Set(28.46, 0);
        edgeShape.m_vertex2.Set(28.46, 15.36);
        body->CreateFixture(&fixtureDef);

        edgeShape.m_vertex1.Set(0.0, 0.0f);
        edgeShape.m_vertex2.Set(28.46, 0.0f);
        body->CreateFixture(&fixtureDef);

        b2Vec2 boxVertices1[4] = {
            b2Vec2(0.0f, 9.9f),
            b2Vec2(0.0f, 10.0f),
            b2Vec2(3.25f, 10.0f),
            b2Vec2(3.25f, 9.9f)
        };

        b2Vec2 boxVertices2[4] = {
            b2Vec2(28.46f, 9.9f),
            b2Vec2(28.46f, 10.0f),
            b2Vec2(28.46f - 3.25f, 10.0f),
            b2Vec2(28.46f - 3.25f, 9.9f)
        };

        b2PolygonShape pShape;
        fixtureDef.shape = &pShape;
        pShape.Set(boxVertices1, 4);
        body->CreateFixture(&fixtureDef);

        pShape.Set(boxVertices2, 4);
        body->CreateFixture(&fixtureDef);
    }

    void handleEvents(){
        while (SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_QUIT:
                exit(1);

            case SDL_KEYDOWN:
                keys[event.key.keysym.scancode] = 2;
                break;
            
            case SDL_KEYUP:
                keys[event.key.keysym.scancode] = 0;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                    printf("%d, %d\n", event.window.data1, event.window.data2);
                    SDL_SetWindowSize(window, 1423, 768);
                    SDL_SetWindowPosition(window, 0, 0);
                }

            case SDL_JOYAXISMOTION:
                //printf("%d, %d\n", event.jaxis.axis, event.jaxis.value);
                break;

            case SDL_JOYBUTTONDOWN:
                //printf("%d\n", event.jbutton.button);
                break;

            default:
                break;
            }
        }
    }

    bool pointInRect(Rect& r, b2Vec2& point){
        if (    point.x > r.x && point.x < r.x + r.w &&
                point.y > r.y && point.y < r.y + r.h)
        {
            return 1;
        }

        return 0;
    }

    static void* threadFunc(void* game){
        Game* g = (Game*)game;
        int extraSteps = 20;
        while (1){
            g->exSem.acquire();
            for (int i = 0; i < extraSteps; i++){
                g->world->Step((1 / 60.0f) / (extraSteps * g->timeMult), 8, 3);
                if (!g->ball->isGoal){
                    g->player1->handle();
                    g->player2->handle();
                }
                g->ball->check();
            }
            g->bSem.release();
        }
    }

    void loop(){
        deltaTimeP = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - lastTimeP;
        lastTimeP = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for (int i = 0; i < SDL_NUM_SCANCODES; i++){
            if (keys[i] == 2) keys[i] = 1;
        }

        handleEvents();

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, bg, NULL, NULL);

        SDL_Rect dest = { 1223, 0, numbers[0].surface->w + 400, numbers[0].surface->h };
        SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
        SDL_RenderFillRect(renderer, &dest);
        dest.w = numbers[player1->score].surface->w;
        dest.h = numbers[player1->score].surface->h;
        SDL_RenderCopy(renderer, numbers[player1->score].texture, NULL, &dest);
        dest.x = 1623;
        dest.w = numbers[player2->score].surface->w;
        dest.h = numbers[player2->score].surface->h;
        SDL_RenderCopy(renderer, numbers[player2->score].texture, NULL, &dest);
        dest.x = (numbers[0].surface->w + 400) / 2.0f + 1223 - dash->surface->w / 2.0f;
        dest.y = -10;
        dest.w = dash->surface->w;
        dest.h = dash->surface->h;
        SDL_RenderCopy(renderer, dash->texture, NULL, &dest);

        ball->trailRender();
        
        bSem.acquire();
        for (GameObject* go : GameObject::gameObjects){
            go->render();
        }

        for (PowerUp* p : PowerUp::aliveUps){
            p->loop();
        }

        ball->render();
        
        goal1->render();
        goal2->render();

        for (PowerUp* p : PowerUp::destroyUps){
            delete(p);
        }

        exSem.release();

        PowerUp::destroyUps.clear();

        SDL_RenderPresent(renderer);
	}
};
