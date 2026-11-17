#ifndef Defeat_H
#define Defeat_H

#include "Boss.h"


class Defeat : public Scene
{
private:

public:
    Defeat();
    Defeat(Vector2 origin, const char *bgHexCode);
    ~Defeat();

    void initialise() override;
    void processInput() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // Defeat_H