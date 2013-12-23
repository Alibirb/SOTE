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

class Weapon;

/// Class for characters
/// TODO: Should probably be called "Character".
class Entity
{
protected:
public:
	std::string name;
	osg::Vec3 position;
	osg::Vec3 initialPosition;

	osg::ref_ptr<osg::Geode> modelNode;
	osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
	float maxSpeed = 3.0f;
	b2Body* physicsBody;
	osg::Vec3 box2DToOsgAdjustment;	/// adjustment between the visual and physical components


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

	virtual void onUpdate(float deltaTime)=0;
};

/**
 * Callback for the entity, used to perform actions that must be performed once per frame.
 */
class EntityUpdateNodeCallback : public osg::NodeCallback
{
private:
	Entity* _owner;
public:

	EntityUpdateNodeCallback(Entity * owner) {
		this->_owner = owner;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};




#endif /* ENTITY_H_ */
