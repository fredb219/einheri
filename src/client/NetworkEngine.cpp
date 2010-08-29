/*
 * ConnexionListener.cpp
 *
 *  Created on: 28 août 2010
 *      Author: fred
 */

#include <iostream>
#include <SFML/Network.hpp>

#include "NetworkEngine.h"
#include "PacketDispatcher.h"
#include "EinheriProtocol.h"
#include "GameEvent.h"
#include "Application.h"


namespace einheri {

NetworkEngine::NetworkEngine(Application *application) {
    app = application;
}

NetworkEngine::~NetworkEngine() {
}


void NetworkEngine::Start(){
    running = true;
    Launch();
}

void NetworkEngine::Stop(){
    running = false;
    Wait();
}

void NetworkEngine::Hello() {
    std::cout<<"Send Hello command"<<std::endl;
        sf::Packet packet;
        int majorProtocolVersion = 45;
        int minorProtocolVersion = 13;
        std::string description = "Einheri alpha - x86_64 GNU/Linux - ubuntu 10.04";
        int command = EinheriProtocol::SERVER_HELLO;

        packet<<command<<majorProtocolVersion<<minorProtocolVersion<<description;
        std::cout<<"command "<<command<<std::endl;
        clientSocket.Send(packet);
}

void NetworkEngine::GetWorld() {
    std::cout<<"Send GetWorld command"<<std::endl;
        sf::Packet packet;

        int command = EinheriProtocol::SERVER_GET_WORLD;
        packet<<command;
        std::cout<<"command "<<command<<std::endl;
        clientSocket.Send(packet);
}


//Private

void NetworkEngine::Run(){
    std::cout<<"NetworkEngine started"<<std::endl;
    sf::SelectorTCP selector;


    PacketDispatcher packetDispatcher(app);

    packetDispatcher.Start();

    if (clientSocket.Connect(5959, "127.0.0.1") != sf::Socket::Done)
    {
        std::cout<<"NetworkEngine failed to connect to 127.0.0.1 port 5959"<<std::endl;
    }

    std::cout<<"NetworkEngine connected to 127.0.0.1 port 5959"<<std::endl;


    selector.Add(clientSocket);

    GameEvent gameEvent;
    gameEvent.type = GameEvent::CONNECTED_TO_SERVER;
    app->gameEngine.SendEvent(gameEvent);


    while(running) {
        unsigned int nbSockets = selector.Wait(1);

        for (unsigned int i = 0; i < nbSockets; ++i)
        {
            sf::SocketTCP socket = selector.GetSocketReady(i);

            sf::Packet packet;
            if (socket.Receive(packet) == sf::Socket::Done)
            {
                //std::cout << "Packet received" << std::endl;
                packetDispatcher.Dispatch(packet, socket);
            }
            else
            {
                std::cout << "A remove client" << std::endl;
                // Erreur : on ferait mieux de retirer le socket du sélecteur
                selector.Remove(socket);
            }

        }

    }
    std::cout<<"NetworkEngine stopping"<<std::endl;

    packetDispatcher.Stop();

    std::cout<<"NetworkEngine stopped"<<std::endl;
}



}
