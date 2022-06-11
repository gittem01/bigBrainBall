#include <GameObject.h>

class PowerUp : public GameObject
{

private:
    long inTimer = 0;
    long lastsFor = 5000; // 5 sec

public:
    static std::vector<PowerUp*> destroyUps;
    static std::set<PowerUp*> aliveUps;

    bool isPicked = false;

    PowerUp(Game* game, const char* texPath);
    ~PowerUp();

    void loop();
    void picked();

    void setLastsFor(long lastsFor);
};