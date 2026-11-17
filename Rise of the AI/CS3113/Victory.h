#ifndef Victory_H
#define Victory_H

#include "Boss.h"


class Victory : public Scene
{
private:

public:
    Victory();
    Victory(Vector2 origin, const char *bgHexCode);
    ~Victory();

    void initialise() override;
    void processInput() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // Victory_H