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
	float maxSpeed = 6.0f;
	StateMachine* _stateMachine;

public:
	std::string name;

#ifndef USE_BOX2D_PHYSICS
	//btKinematicCharacterController* controller;
	ImprovedBulletKinematicCharacterController* controller;
#endif

	Entity();
	Entity(std::string name, osg::Vec3 position);
	virtual ~Entity();

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual void setPosition(osg::Vec3 newPosition);

	double getHeading();
	void setHeading(double angle);

	const osg::Quat& getAttitude();
	void setAttitude(const osg::Quat& newAttitude);

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
};

void registerEntity();

#endif /* ENTITY_H_ */
