#include <PowerUp.h>

std::vector<PowerUp*> PowerUp::destroyUps;
std::set<PowerUp*> PowerUp::aliveUps;

PowerUp::PowerUp(Game* game, const char* texPath) : GameObject(game, texPath){
    this->body->GetFixtureList()->SetSensor(true);
    this->body->GetFixtureList()->SetDensity(0.0f);
    this->body->ResetMassData();

    GameObject::gameObjects.erase(GameObject::gameObjects.end() - 1);
    aliveUps.insert(this);
}

PowerUp::~PowerUp(){
    aliveUps.erase(this);
}

void PowerUp::picked(){
    isPicked = true;
    inTimer = SDL_GetTicks();

    // start efects here
}

void PowerUp::loop(){
    if (isPicked && SDL_GetTicks() - inTimer > 2000){
        destroyUps.push_back(this);

        // end efects here
    }
    if (isPicked){
        return;
    }

    render();


    // TODO : give bodydata to physical bodies and filter pickable bodies
    b2ContactEdge* contact = body->GetContactList();
    if (contact != NULL && (!contact->contact->GetFixtureA()->IsSensor() || !contact->contact->GetFixtureB()->IsSensor())){
        picked();
    }
}

void PowerUp::setLastsFor(long lastsFor){
    this->lastsFor = lastsFor;
}