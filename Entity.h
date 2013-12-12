/*
 * Entity.h
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <osg/PositionAttitudeTransform>

#include "globals.h"
#include "Box2DIntegration.h"


#define DEFAULT_ENTITY_IMAGE "test.png"



class Entity
{
public:
	std::string name;
	osg::Vec3 position;
	osg::Vec3 initialPosition;

	osg::Geode *modelNode;
	osg::PositionAttitudeTransform *transformNode;
	float maxSpeed = 3.0f;
	b2Body *physicsBody;
	osg::Vec3 box2DToOsgAdjustment;	//adjustment between the visual and physical components


	enum State {
		awake, napping, dead
	};

	State state;

	Entity(std::string name, osg::Vec3 position, std::string imageFilename = DEFAULT_ENTITY_IMAGE);

	virtual void jump();

	virtual void resetPosition() {
		this->setPosition(initialPosition);
	}

	virtual ~Entity();

	virtual osg::Vec3 getPosition() {
		return transformNode->getPosition();
	}
	virtual void setPosition(osg::Vec3 newPosition) {
		transformNode->setPosition(newPosition);
		physicsBody->SetTransform(toB2Vec2(newPosition - box2DToOsgAdjustment), physicsBody->GetAngle());
	}

};


#endif /* ENTITY_H_ */
