/*
 * PacketDispatcher.cpp
 *
 *  Created on: 29 août 2010
 *      Author: fred
 */
#include <iostream>

#include "PacketDispatcher.h"
#include "../common/EinheriProtocol.h"
#include "GameEvent.h"
#include "Application.h"
#include "world/Bullet.h"

namespace einheri {

PacketDispatcher::PacketDispatcher(Application *application) {
    app = application;
}

PacketDispatcher::~PacketDispatcher() {
    // TODO Auto-generated destructor stub
}

void PacketDispatcher::Start() {
    running = true;
    Launch();
}

void PacketDispatcher::Stop() {
    running = false;
    sf::Packet packet;
    sf::SocketTCP socket;
    packetQueue.PushMessage(packet);
    Wait();
}

void PacketDispatcher::Dispatch(sf::Packet packet, sf::SocketTCP /*socket*/) {
    packetQueue.PushMessage(packet);
}

//Private

void PacketDispatcher::Run() {
    std::cout << "PacketDispatcher started" << std::endl;

    sf::Packet packet = packetQueue.PopMessage();

    while (running) {
        EinheriProtocol::ClientCommandType commandType;
        int commandTypeInt;
        packet >> commandTypeInt;

        commandType = (EinheriProtocol::ClientCommandType) commandTypeInt;

        //std::cout<<"PacketDispatcher command "<<EinheriProtocol::getCommandName(commandType)<<std::endl;

        if (!packet) {
            std::cout << "PacketDispatcher bad format" << std::endl;
        }

        switch (commandType) {
        case EinheriProtocol::CLIENT_HELLO:
            dispatchClientHello(&packet);
            break;
        case EinheriProtocol::CLIENT_GOODBYE:
            //TODO
            break;
        case EinheriProtocol::CLIENT_ADD_MONSTER:
            dispatchClientAddMonster(&packet);
            break;
        case EinheriProtocol::CLIENT_ADD_MONSTERS:
            dispatchClientAddMonsters(&packet);
            break;
        case EinheriProtocol::CLIENT_UPDATE_MONSTER:
            dispatchClientUpdateMonster(&packet);
            break;
        case EinheriProtocol::CLIENT_UPDATE_MONSTERS:
            dispatchClientUpdateMonsters(&packet);
            break;
        case EinheriProtocol::CLIENT_ADD_HERO:
            dispatchClientAddHero(&packet);
            break;
        case EinheriProtocol::CLIENT_ADD_HEROES:
            dispatchClientAddHeroes(&packet);
            break;
        case EinheriProtocol::CLIENT_UPDATE_HERO:
            dispatchClientUpdateHero(&packet);
            break;
        case EinheriProtocol::CLIENT_UPDATE_HERO_AIMING_ANGLE:
            dispatchClientUpdateHeroAimingAngle(&packet);
            break;
        case EinheriProtocol::CLIENT_UPDATE_HEROES:
            dispatchClientUpdateHeroes(&packet);
            break;
        case EinheriProtocol::CLIENT_PLAYER_ADDED:
            dispatchClientPlayerAdded(&packet);
            break;
        case EinheriProtocol::CLIENT_HERO_ADDED:
            dispatchClientHeroAdded(&packet);
            break;
        case EinheriProtocol::CLIENT_ADD_PROJECTILE:
            dispatchClientAddProjectile(&packet);
                    break;
        case EinheriProtocol::CLIENT_UPDATE_PROJECTILE:
            dispatchClientUpdateProjectile(&packet);
                            break;
        }

        if (!packet) {
            std::cout << "PacketDispatcher bad format" << std::endl;
        }

        packet = packetQueue.PopMessage();
    }

    std::cout << "PacketDispatcher stopped" << std::endl;
}

void PacketDispatcher::dispatchClientHello(sf::Packet *packet) {
    int majorProtocolVersion;
    int minorProtocolVersion;
    std::string description;

    (*packet) >> majorProtocolVersion >> minorProtocolVersion >> description;
    std::cout << "Server connected. Protocol version : "
            << majorProtocolVersion << "." << minorProtocolVersion
            << ". Description : " << description << std::endl;

    GameEvent gameEvent(GameEvent::REGISTERED_TO_SERVER);
    app->gameEngine.SendEvent(gameEvent);

}

void PacketDispatcher::dispatchClientAddMonsters(sf::Packet *packet) {

    int count;

    (*packet) >> count;
    std::cout << "Add monsters " << count << std::endl;

    app->clientWorldEngine.worldModel->mutexMonsters.Lock();
    for (int i = 0; i < count; i++) {
        int id;
        (*packet) >> id;
        //std::cout<<"Add monster "<<id<<std::endl;
        Monster *monster = new Monster();
        monster->id = id;
        app->clientWorldEngine.worldModel->AddMonster(monster);
    }
    app->clientWorldEngine.worldModel->mutexMonsters.Unlock();
}

void PacketDispatcher::dispatchClientAddMonster(sf::Packet *packet) {
    int id;

    (*packet) >> id;
    //std::cout << "Add monster " << id << std::endl;

    Monster *monster = new Monster();
    monster->id = id;

    app->clientWorldEngine.worldModel->mutexMonsters.Lock();

    app->clientWorldEngine.worldModel->AddMonster(monster);

    app->clientWorldEngine.worldModel->mutexMonsters.Unlock();
}

void PacketDispatcher::dispatchClientUpdateMonster(sf::Packet *packet) {
    int id;
    double speedX;
    double speedY;
    double posX;
    double posY;
    double angle;

    (*packet) >> id >> speedX >> speedY >> posX >> posY >> angle;
    //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

    app->clientWorldEngine.worldModel->mutexMonsters.Lock();

    if (app->clientWorldEngine.worldModel->GetMonsters().count(id) > 0) {
        Monster *monster = app->clientWorldEngine.worldModel->GetMonsters().at(
                id);
        monster->angle = angle;
        monster->speedX = speedX;
        monster->speedY = speedY;
        monster->positionX = posX;
        monster->positionY = posY;
    } else {
        std::cout << "Error missing id Updating monster " << id << " sx"
                << speedX << " sy" << speedY << " px" << posX << " py" << posY
                << " a" << angle << " " << std::endl;
    }

    app->clientWorldEngine.worldModel->mutexMonsters.Unlock();
}



void PacketDispatcher::dispatchClientUpdateMonsters(sf::Packet *packet) {

    int count;

    (*packet) >> count;
    //std::cout << "Update monsters " << count << std::endl;

    app->clientWorldEngine.worldModel->mutexMonsters.Lock();
    for (int i = 0; i < count; i++) {
        int id;
        double speedX;
        double speedY;
        double posX;
        double posY;
        double angle;

        (*packet) >> id >> speedX >> speedY >> posX >> posY >> angle;
        //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

        if (app->clientWorldEngine.worldModel->GetMonsters().count(id) > 0) {
            Monster *monster =
                    app->clientWorldEngine.worldModel->GetMonsters().at(id);
            monster->angle = angle;
            monster->speedX = speedX;
            monster->speedY = speedY;
            monster->positionX = posX;
            monster->positionY = posY;
        } else {
            std::cout << "Error missing id Updating monster " << id << " sx"
                    << speedX << " sy" << speedY << " px" << posX << " py"
                    << posY << " a" << angle << " " << std::endl;
        }

    }
    app->clientWorldEngine.worldModel->mutexMonsters.Unlock();
}

void PacketDispatcher::dispatchClientUpdateHero(sf::Packet *packet) {
    int id;
    double speedX;
    double speedY;
    double posX;
    double posY;
    double angle;

    (*packet) >> id >> speedX >> speedY >> posX >> posY >> angle;
    //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

    app->clientWorldEngine.worldModel->mutexHeroes.Lock();

    if (app->clientWorldEngine.worldModel->GetHeroes().count(id) > 0) {
        Hero *hero = app->clientWorldEngine.worldModel->GetHeroes().at(
                id);
        hero->angle = angle;
        hero->speedX = speedX;
        hero->speedY = speedY;
        hero->positionX = posX;
        hero->positionY = posY;
    } else {
        std::cout << "Error missing id Updating hero " << id << " sx"
                << speedX << " sy" << speedY << " px" << posX << " py" << posY
                << " a" << angle << " " << std::endl;
    }

    app->clientWorldEngine.worldModel->mutexHeroes.Unlock();
}

void PacketDispatcher::dispatchClientUpdateHeroAimingAngle(sf::Packet *packet) {
    int id;
    double angle;

    (*packet) >> id >>  angle;
    //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

    app->clientWorldEngine.worldModel->mutexHeroes.Lock();

    if (app->clientWorldEngine.worldModel->GetHeroes().count(id) > 0) {
        Hero *hero = app->clientWorldEngine.worldModel->GetHeroes().at(
                id);
        hero->aimingAngle = angle;

    } else {
        std::cout << "Error missing id Updating hero aiming angle" << id << " aiming angle="
                << angle << std::endl;
    }

    app->clientWorldEngine.worldModel->mutexHeroes.Unlock();
}

void PacketDispatcher::dispatchClientUpdateHeroes(sf::Packet *packet) {

    int count;

    (*packet) >> count;
    //std::cout << "Update monsters " << count << std::endl;

    app->clientWorldEngine.worldModel->mutexHeroes.Lock();
    for (int i = 0; i < count; i++) {
        int id;
        double speedX;
        double speedY;
        double posX;
        double posY;
        double angle;

        (*packet) >> id >> speedX >> speedY >> posX >> posY >> angle;
        //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

        if (app->clientWorldEngine.worldModel->GetHeroes().count(id) > 0) {
            Hero *hero =
                    app->clientWorldEngine.worldModel->GetHeroes().at(id);
            hero->angle = angle;
            hero->speedX = speedX;
            hero->speedY = speedY;
            hero->positionX = posX;
            hero->positionY = posY;
        } else {
            std::cout << "Error missing id Updating hero " << id << " sx"
                    << speedX << " sy" << speedY << " px" << posX << " py"
                    << posY << " a" << angle << " " << std::endl;
        }

    }
    app->clientWorldEngine.worldModel->mutexHeroes.Unlock();
}

void PacketDispatcher::dispatchClientAddHero(sf::Packet *packet) {
    int id;

    (*packet) >> id;
    //std::cout << "Add monster " << id << std::endl;

    Hero *hero = new Hero();
    hero->id = id;

    app->clientWorldEngine.worldModel->mutexHeroes.Lock();

    app->clientWorldEngine.worldModel->AddHero(hero);

    app->clientWorldEngine.worldModel->mutexHeroes.Unlock();
}

void PacketDispatcher::dispatchClientAddHeroes(sf::Packet *packet) {

    int count;

    (*packet) >> count;
    std::cout << "Add heroes " << count << std::endl;

    app->clientWorldEngine.worldModel->mutexHeroes.Lock();
    for (int i = 0; i < count; i++) {
        int id;
        (*packet) >> id;
        //std::cout<<"Add monster "<<id<<std::endl;
        Hero *hero = new Hero();
        hero->id = id;
        app->clientWorldEngine.worldModel->AddHero(hero);
    }
    app->clientWorldEngine.worldModel->mutexHeroes.Unlock();
}

void PacketDispatcher::dispatchClientPlayerAdded(sf::Packet *packet) {
    int id;

    (*packet) >> id;

    std::cout << "PacketDispatcher::dispatchClientPlayerAdded" << id << std::endl;
    GameEvent gameEvent(GameEvent::PLAYER_ADDED);
    gameEvent.intValues[GameEvent::PLAYER_ID] = id;
    app->gameEngine.SendEvent(gameEvent);
}

void PacketDispatcher::dispatchClientHeroAdded(sf::Packet *packet) {
    int playerId;
    int heroId;

    (*packet) >> playerId >> heroId;

    std::cout << "PacketDispatcher::dispatchClientHeroAdded" << playerId << " " << heroId<< std::endl;
    GameEvent gameEvent(GameEvent::HERO_ADDED);
    gameEvent.intValues[GameEvent::PLAYER_ID] = playerId;
    gameEvent.intValues[GameEvent::HERO_ID] = heroId;
    app->gameEngine.SendEvent(gameEvent);
}

void PacketDispatcher::dispatchClientAddProjectile(sf::Packet *packet) {
    int projectileId;
    int projectileTypeInt;
    (*packet) >> projectileId >> projectileTypeInt;
    //std::cout << "Add monster " << id << std::endl;

    EinheriProtocol::ProjectileType projectileType = (EinheriProtocol::ProjectileType) projectileTypeInt;


    Projectile *newProjectile;

    if(projectileType == EinheriProtocol::PROJECTILE_BULLET) {
        newProjectile = new Bullet();
    }

    newProjectile->id = projectileId;

    app->clientWorldEngine.worldModel->mutexProjectiles.Lock();

    app->clientWorldEngine.worldModel->AddProjectile(newProjectile);

    app->clientWorldEngine.worldModel->mutexProjectiles.Unlock();
}

void PacketDispatcher::dispatchClientUpdateProjectile(sf::Packet *packet) {
    int id;
    double speedX;
    double speedY;
    double posX;
    double posY;
    double angle;

    (*packet) >> id >> speedX >> speedY >> posX >> posY >> angle;
    //std::cout<<"Update monster "<<id<<" sx"<<speedX<<" sy"<<speedY<<" px"<<posX<<" py"<<posY<<" a"<<angle<<" "<<std::endl;

    app->clientWorldEngine.worldModel->mutexProjectiles.Lock();

    if (app->clientWorldEngine.worldModel->GetProjectiles().count(id) > 0) {
        Projectile *projectile = app->clientWorldEngine.worldModel->GetProjectiles().at(
                id);
        projectile->angle = angle;
        projectile->speedX = speedX;
        projectile->speedY = speedY;
        projectile->positionX = posX;
        projectile->positionY = posY;
    } else {
        std::cout << "Error missing id Updating projectile " << id << " sx"
                << speedX << " sy" << speedY << " px" << posX << " py" << posY
                << " a" << angle << " " << std::endl;
    }

    app->clientWorldEngine.worldModel->mutexProjectiles.Unlock();
}


}
