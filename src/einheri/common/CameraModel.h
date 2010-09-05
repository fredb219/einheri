/*
 * Model.h
 *
 *  Created on: 4 sept. 2010
 *      Author: fred
 */

#ifndef EIN_CAMERA_MODEL_H_
#define EIN_CAMERA_MODEL_H_

#include <einheri/common/Vect2.h>

namespace ein {

class CameraModel {
public:
    CameraModel();
    virtual ~CameraModel();

    EinValue GetDistance() const { return distance; }
    Vector GetPosition() const { return position; }

    void SetDistance(EinValue newDistance) {distance = newDistance; }
    void SetDistance(Vector newPosition) {position = newPosition; }

private:
    EinValue distance;
    Vector position;

};

}

#endif /* EIN_CAMERA_MODEL_H_ */
