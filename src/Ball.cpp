#include <Ball.h>
#include <Game.h>

Ball::Ball(Game* game) : GameObject(game, "../assets/footer.png", b2Shape::e_circle, b2Vec2(0.3f, 1))
{
    startThrow();
    body->SetLinearDamping(0.2f);
    body->GetFixtureList()->SetRestitution(0.75f);
    body->GetFixtureList()->SetFriction(0.5f);
    body->SetBullet(true);
    b2Filter filter;
    filter.categoryBits = C4;
    filter.maskBits = C1 | C2 | C3 | C4;
    body->GetFixtureList()->SetFilterData(filter);

    for (int i = 0; i < TRAIL_SIZE; i++){
        ballTrail[i] = b2Vec2(-10, -10);
    }

    SDL_Surface* s = IMG_Load("../assets/trail.png");
    trail = SDL_CreateTextureFromSurface(renderer, s);

    GameObject::gameObjects.erase(GameObject::gameObjects.end() - 1);
}

// 0 for p1 goal enter, 1 for p2 goal enter
// 1 left, 0 right goal
int Ball::checkBallGoal(){
    b2Vec2 bPos = body->GetPosition();
    if (game->pointInRect(game->goalRects[0], bPos))
    {
        return 1;
    }
    else if (game->pointInRect(game->goalRects[1], bPos))
    {
        return 0;
    }

    return -1;
}

void Ball::check(){
    b2Vec2 p = body->GetPosition();
    if (game->pointInRect(game->topRects[0], p) && abs(body->GetAngularVelocity()) < 5.0f){
        body->SetAngularVelocity(+0.1f + body->GetAngularVelocity());
    }
    else if (game->pointInRect(game->topRects[1], p) && abs(body->GetAngularVelocity()) < 5.0f){
        body->SetAngularVelocity(-0.1f + body->GetAngularVelocity());
    }

    long t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (t - lastTrailTime > 5 * game->timeMult * game->timeMult){
        lastTrailTime = t;
        ballTrail[ballTrailIndex] = body->GetPosition();
        ballTrailIndex = (ballTrailIndex + 1) % TRAIL_SIZE;
    }

    int ballStat = checkBallGoal();
    if (ballStat != -1 && !isGoal){
        if (ballStat == 0){
            game->player1->score++;
        }
        else if (ballStat == 1){
            game->player2->score++;
        }
        
        isGoal = true;
        goalTimer = SDL_GetTicks();
        game->timeMult = 8.0f;
    }

    if (isGoal && SDL_GetTicks() - goalTimer > 2000){
        isGoal = false;
        game->timeMult = 1.0f;
        startThrow();

        game->player1->body->SetTransform(game->player1->startPositions[game->player1->playerNum], 0);
        game->player2->body->SetTransform(game->player2->startPositions[game->player2->playerNum], 0);

        game->player1->setFoot();
        game->player2->setFoot();
    }
}

void Ball::startThrow(){
    body->SetTransform(b2Vec2(14, 7), 0);
    body->SetLinearVelocity(b2Vec2(rand() % 20 - 10, -rand() % 10 - 5));
    body->SetAngularVelocity((rand() % 100) / 20.0f);
}

void Ball::trailRender(){
    for (int i = 0; i < TRAIL_SIZE; i++){
        int p = (ballTrailIndex + i) % TRAIL_SIZE;
        int w = i / (TRAIL_SIZE / 50);
        int h = i / (TRAIL_SIZE / 50);
        SDL_Rect trailRect = { (int)(ballTrail[p].x * 100 - w / 2), (int)(ballTrail[p].y * 100 - h / 2), w, h };
        SDL_RenderCopy(renderer, trail, NULL, &trailRect);
    }
}
