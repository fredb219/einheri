/*
 * GameEngine.h
 *
 *  Created on: 21 août 2010
 *      Author: fred
 */

#ifndef GAMEENGINE_H_
#define GAMEENGINE_H_

#include "GameEvent.h"
#include "Player.h"

namespace einheri {

class ClientWorldModel;
class Application;


class GameEngine : private sf::Thread {
public:
    GameEngine(Application *application);
    virtual ~GameEngine();

    void Start();
    void Stop();

    void SendEvent(GameEvent event);

    Player localPlayer;

private:
    bool running;
    Application *application;

    virtual void Run();

    virtual void processEvent();


    MessageQueue<GameEvent> events;
};

}

#endif /* GAMEENGINE_H_ */
