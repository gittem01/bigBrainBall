#include <Player.h>

Player::Player(b2World* world, SDL_Renderer* renderer, const char* texPath, int pNum):
        GameObject(world, renderer, texPath, b2Shape::Type::e_circle, b2Vec2(0.7f, 1.0f), {}, 1.0f)
{
    playerNum = pNum;

    flip = (SDL_RendererFlip)((int)(isFlipped[pNum]) | ((int)flip));

    body->SetTransform(startPositions[pNum], 0);

    foot = new GameObject(world, renderer, "../assets/shoe.png", b2Shape::e_polygon, b2Vec2(0.7f, 0.7f), footVertices[pNum]);
    foot->body->GetFixtureList()->SetRestitution(0.0f);
    foot->body->GetFixtureList()->SetDensity(0.1f);
    foot->body->ResetMassData();
    foot->flip = (SDL_RendererFlip)((int)(isFlipped[pNum]) | ((int)flip));

    b2Filter filter;
    filter.categoryBits = C3;
    filter.maskBits = C4;

    foot->body->GetFixtureList()->SetFilterData(filter);

    j = NULL;

    setFoot();
}

void Player::setFoot(){
    if (j){
        world->DestroyJoint(j);
    }
    
    foot->body->SetTransform(b2Vec2(body->GetPosition().x + footInc[playerNum], body->GetPosition().y + 0.5f), 0);

    b2RevoluteJointDef jDef;
    jDef.upperAngle = footLimits[playerNum][0];
    jDef.lowerAngle = footLimits[playerNum][1];
    jDef.enableLimit = true;
    jDef.maxMotorTorque = 10000000;
    jDef.enableMotor = true;
    jDef.Initialize(body, foot->body, body->GetPosition());

    j = (b2RevoluteJoint*)world->CreateJoint(&jDef);
}

void Player::setKeys(int* keys){
    this->keys = keys;
}

void Player::handle(){
    b2Vec2 lastVel = body->GetLinearVelocity();

    bool pressed = false;
    if (keys[playerKeys[playerNum][in_RIGHT]]){
        pressed ^= true;
        lastVel.x += 1.0f;
    }
    if (keys[playerKeys[playerNum][in_LEFT]]){
        pressed ^= true;
        lastVel.x -= 1.0f;
    }

    if (keys[playerKeys[playerNum][in_UP]] && abs(lastVel.y - lastySpeed) < 0.0001f){
        lastVel.y = -12.0f;
    }

    if (lastVel.x > 6.0f){
        lastVel.x = 6.0f;
    }
    else if (lastVel.x < -6.0f){
        lastVel.x = -6.0f;
    }

    if (!pressed){
        lastVel.x *= 0.98f;
    }

    body->SetLinearVelocity(lastVel);

    lastySpeed = lastVel.y;

    if (keys[playerKeys[playerNum][in_SPACE]]){
        // releases the "tension?"
        j->EnableLimit(!j->IsEnabled());
        j->EnableLimit(true);

        j->SetMotorSpeed(20.0f * playerMultipliers[playerNum]);
    }
    else if (!(abs(j->GetJointAngle() - footLimits[playerNum][playerNum ^ 1]) < 0.001f)){
        j->EnableLimit(!j->IsEnabled());
        j->EnableLimit(true);

        j->SetMotorSpeed(-10.0f * playerMultipliers[playerNum]);
    }
}