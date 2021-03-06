/*
 * Movable.h
 *
 *  Created on: 4 sept. 2010
 *      Author: fred
 */

#ifndef EHR_COMMON_MOVABLE_H_
#define EHR_COMMON_MOVABLE_H_

#include <einheri/common/Vect2.h>
#include <einheri/utils/Visitor.h>
#include <string>

namespace ein {
class CollisionVisitor;

class Movable: public einUtils::VisitableBase<Movable> {
public:
    EIN_DEFINE_VISITABLE()
	
	
	enum MovableType {
		HERO,
		MONSTER
	};
	
public:
    Movable();
    virtual ~Movable();

    std::string GetName() const {
        return name;
    }

    void SetName(std::string name) {
        this->name = name;
    }

    EinId GetId() const {
        return id;
    }
    
    void SetId(EinId id) {
        this->id = id;
    }

    Vector GetPosition() const {
        return position;
    }

    Vector GetSpeed() const {
        return speed;
    }

    Vector GetTargetedSpeed() const {
        return targetedSpeed;
    }

    EinValue GetAngle() const {
        return angle;
    }

    bool IsNeedReportingCollision() const {
        return needReportingCollision;
    }

    void SetPosition(Vector newPosition) {
        position = newPosition;
    }

    void SetSpeed(Vector newSpeed) {
        speed = newSpeed;
    }

    void SetTargetedSpeed(Vector newSpeed) {
        targetedSpeed = newSpeed;
    }

    void SetAngle(EinValue newAngle) {
        angle = newAngle;
    }

    void SetNeedReportingCollision(bool needReportingCollision) {
        this->needReportingCollision = needReportingCollision;
    }

    bool IsAlive() {
        return alive;
    }

    void SetAlive(bool alive) {
        this->alive = alive;
    }

    EinValue GetRadius() const {
        return radius;
    }
    void SetRadius(EinValue newRadius) {
        this->radius = newRadius;
    }
    
    EinValue GetType() const {
        return type;
    }
    
    void SetType(MovableType type) {
		this->type = type;
	}

private:
    EinId id;

    Vector position;
    Vector speed;
    Vector targetedSpeed;
    EinValue angle;
    EinValue radius;
    bool needReportingCollision;
    std::string name;
	MovableType type;
    bool alive;
};

}

#endif /* EHR_COMMON_MOVABLE_H_ */
