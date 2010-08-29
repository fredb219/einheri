/*
 * ClientWorldModel.h
 *
 *  Created on: 21 août 2010
 *      Author: fred
 */

#ifndef WORLDMODEL_H_
#define WORLDMODEL_H_

#include <vector>
#include <SFML/System.hpp>
#include "Hero.h"
#include "Monster.h"
#include "Map.h"

namespace einheri {

class WorldModel {
public:
    WorldModel();
    virtual ~WorldModel();

    void Lock();
    void Unlock();

    Map map;
    std::vector<Hero *> heroes;
    std::vector<Monster *> monsters;

private:
    sf::Mutex lock;

};

}

#endif /* WORLDMODEL_H_ */
