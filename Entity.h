/*
 * Entity.h
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "GameObject.h"

#include <osg/PositionAttitudeTransform>

#include "globals.h"

#ifdef USE_BOX2D_PHYSICS
	#include "Box2DIntegration.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "BulletDynamics/Character/btKinematicCharacterController.h"
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif



/// Class for characters
/// TODO: Should probably be called "Character".
class Entity : public GameObject
{
protected:
	float maxSpeed = 6.0f;

public:
	std::string name;
#ifndef USE_BOX2D_PHYSICS
	btKinematicCharacterController* controller;
#endif

	enum State {
		awake, napping, dead
	};

	State state;

	Entity(std::string name, osg::Vec3 position);

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual ~Entity();

	virtual void setPosition(osg::Vec3 newPosition);

	double getHeading();
	void setHeading(double angle);

	const osg::Quat& getAttitude();
	void setAttitude(const osg::Quat& newAttitude);

	virtual void onUpdate(float deltaTime)=0;
};



#endif /* ENTITY_H_ */
