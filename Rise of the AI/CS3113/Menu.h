#ifndef MENU_H
#define MENU_H

#include "Scene.h"


class Menu : public Scene
{
private:

public:
    Menu();
    Menu(Vector2 origin, const char *bgHexCode);
    ~Menu();

    void initialise() override;
    void processInput() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // MENU_H