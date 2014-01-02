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
#include "Weapon.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "OwnerUpdateCallback.h"


Entity::Entity(std::string name, osg::Vec3 position)
{
	initialPosition = position;

	this->name = name;
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->setPosition(position);

	modelNode = new Sprite();
	//modelNode->setUpdateCallback(new EntityUpdateNodeCallback(this));
	modelNode->setUpdateCallback(new OwnerUpdateCallback<Entity>(this));
	root->addChild(transformNode);
	transformNode->addChild(modelNode);


	box2DToOsgAdjustment = osg::Vec3(0.0, 0.0, 0.0);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x() - box2DToOsgAdjustment.x() , position.y() - box2DToOsgAdjustment.y());
	physicsBody = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);

	// Create fixture for obstacle collision (around the feet)
	b2FixtureDef bodyFixtureDef;
	b2CircleShape bodyShape;
	bodyShape.m_radius = .125f;
	bodyShape.m_p = b2Vec2(0, -.4);
	bodyFixtureDef.shape = &bodyShape;
	bodyFixtureDef.filter.categoryBits = CollisionCategories::OBSTACLE;
	bodyFixtureDef.filter.maskBits = CollisionCategories::ALL;
	physicsBody->CreateFixture(&bodyFixtureDef);

	// Create hit box
	b2FixtureDef hitBoxFixtureDef;
	b2PolygonShape hitBoxShape;
	hitBoxShape.SetAsBox(.125f, .5f, b2Vec2(0, 0), 0.0);
	hitBoxFixtureDef.shape = &hitBoxShape;
	hitBoxFixtureDef.filter.categoryBits = CollisionCategories::HIT_BOX;
	hitBoxFixtureDef.filter.maskBits = CollisionCategories::PAIN_SOURCE;
	b2Fixture *hitBoxFixture = physicsBody->CreateFixture(&hitBoxFixtureDef);

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

void Entity::setSpriteImage(std::string imageFilename)
{
	//dynamic_cast<Sprite*>(modelNode)->setImage(imageFilename);
	dynamic_pointer_cast<Sprite>(modelNode)->setImage(imageFilename);
}

void Entity::jump()
{

}



