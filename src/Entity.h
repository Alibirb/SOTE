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
	//#include "BulletDynamics/Character/btKinematicCharacterController.h"
	#include "ImprovedBulletKinematicCharacterController.h"
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif


#include "StateMachine.h"


/// Class for characters
/// TODO: Should probably be called "Character".
class Entity : public GameObject
{
protected:
	float _maxSpeed = 6.0f;
	StateMachine* _stateMachine;

	float _capsuleRadius;
	float _capsuleHeight;

	std::string _name;

public:
#ifndef USE_BOX2D_PHYSICS
	ImprovedBulletKinematicCharacterController* _controller;
#endif

	Entity(osg::Group* parentNode);
	virtual ~Entity();

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual void setPosition(osg::Vec3 newPosition);


	double getHeading();
	void setHeading(double angle);

	std::string getName();

	virtual void onUpdate(float deltaTime){};

	StateMachine* getStateMachine();
	void changeState(std::string& stateName);
	std::string getCurrentStateName();
	void returnToPreviousState();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveEntityVariables(GameObjectData* data);	/// Saves the variables declared in Entity.
	void loadEntityVariables(GameObjectData* data);	/// Loads the variables declared in Entity.

	void createController();
};

void registerEntity();

#endif /* ENTITY_H_ */
