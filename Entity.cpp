/*
 * Entity.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#include "Entity.h"
#include "PhysicsNodeCallback.h"
#include "Level2D.h"
#include "Sprite.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>


Entity::Entity(std::string name, osg::Vec3 position, std::string imageFilename)
{
	initialPosition = position;

	this->name = name;
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->setPosition(position);

	modelNode = new Sprite(imageFilename);
	root->addChild(transformNode);
	transformNode->addChild(modelNode);

	//box2DToOsgAdjustment = osg::Vec3(-0.5, -0.5, 0.0);
	box2DToOsgAdjustment = osg::Vec3(0.0, 0.0, 0.0);

	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.position.Set(position.x() - box2DToOsgAdjustment.x() , position.y() - box2DToOsgAdjustment.y());
	physicsBody = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);
	b2PolygonShape collisionBox;
	collisionBox.SetAsBox(.5f, .5f);	// HALF-extents, remember.
	physicsBody->CreateFixture(&collisionBox, 0.0f);
	Box2DUserData *userData = new Box2DUserData;
	userData->owner = this;
	userData->ownerType = "Entity";
	physicsBody->SetUserData(userData);

	transformNode->setUpdateCallback(new PhysicsNodeCallback(transformNode, physicsBody, box2DToOsgAdjustment));


	state = awake;
}

Entity::~Entity()
{
	transformNode->getParent(0)->removeChild(transformNode);	// remove the node from the scenegraph.
	getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
}

void Entity::jump()
{

}
