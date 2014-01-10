/*
 * Entity.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#include "Entity.h"
#include "PhysicsNodeCallback.h"
#include "Level.h"

#include "Sprite.h"
#include "Weapon.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "OwnerUpdateCallback.h"

#define DEFAULT_ENTITY_MODEL_NAME "humanmodelNoBones.osg"


Entity::Entity(std::string name, osg::Vec3 position)
{
	initialPosition = position;

	this->name = name;
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->setPosition(position);

	if(_useSpriteAsModel)
		modelNode = new Sprite();
	else
	{
		std::string modelFilename = DEFAULT_ENTITY_MODEL_NAME;
		modelNode = osgDB::readNodeFile(modelFilename);
	}
	modelNode->setUpdateCallback(new OwnerUpdateCallback<Entity>(this));

	addToSceneGraph(transformNode);
	transformNode->addChild(modelNode);

#ifdef USE_BOX2D_PHYSICS
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
#else
	float capsuleHeight = 1.25;
	float capsuleRadius = .4;
	physicsToModelAdjustment = osg::Vec3(0, 0, capsuleHeight/2 + capsuleRadius);
	btCapsuleShapeZ* shape = new btCapsuleShapeZ(capsuleRadius, capsuleHeight);

	btTransform transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(osgbCollision::asBtVector3(position + physicsToModelAdjustment));
	btPairCachingGhostObject * ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(transform);
	getCurrentLevel()->getBulletWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	ghostObject->setCollisionShape(shape);
	ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	float stepHeight = .35;
	controller = new btKinematicCharacterController(ghostObject, shape, stepHeight, 2);
	controller->setGravity(-getCurrentLevel()->getBulletWorld()->getGravity().z());
	//getCurrentLevel()->getBulletWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	getCurrentLevel()->getBulletWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
	getCurrentLevel()->getBulletWorld()->addAction(controller);

	transformNode->setUpdateCallback(new BulletPhysicsNodeCallback(ghostObject, -physicsToModelAdjustment));
#endif

	state = awake;
}

Entity::~Entity()
{
	transformNode->getParent(0)->removeChild(transformNode);	// remove the node from the scenegraph.
#ifdef USE_BOX2D_PHYSICS
	getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
#else
	getCurrentLevel()->getBulletWorld()->removeCollisionObject(controller->getGhostObject());
	getCurrentLevel()->getBulletWorld()->removeAction(controller);
	delete controller;
#endif
}

void Entity::loadModel(std::string modelFilename)
{
	if(_useSpriteAsModel)
		dynamic_pointer_cast<Sprite>(modelNode)->setImage(modelFilename);
	else
	{
		transformNode->removeChild(modelNode);
		modelNode = osgDB::readNodeFile(modelFilename);
		modelNode->setUpdateCallback(new OwnerUpdateCallback<Entity>(this));
		transformNode->addChild(modelNode);
	}
}

void Entity::jump()
{
#ifndef USE_BOX2D_PHYSICS
	controller->jump();
#endif // USE_BOX2D_PHYSICS
}



