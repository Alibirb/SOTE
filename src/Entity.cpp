/*
 * Entity.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: daniel
 */

#include "Entity.h"
#include "PhysicsNodeCallback.h"
#include "PhysicsData.h"
#include "Level.h"

#include "Sprite.h"
#include "Weapon.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "OwnerUpdateCallback.h"
#include "GameObjectData.h"

#include "StateMachine.h"

#include "AngelScriptEngine.h"


Entity::Entity(osg::Group* parentNode) : GameObject(parentNode), _controller(NULL)
{
	registerEntity();
	osg::Vec3 position;
	_stateMachine = new StateMachine(this);

	createController();
}



Entity::~Entity()
{
#ifndef USE_BOX2D_PHYSICS
	getCurrentLevel()->getBulletWorld()->removeAction(_controller);
	delete _controller;
#endif
}

void Entity::jump()
{
#ifndef USE_BOX2D_PHYSICS
	_controller->jump();
#endif // USE_BOX2D_PHYSICS
}

void Entity::setPosition(osg::Vec3 newPosition)
{
	_transformNode->setPosition(newPosition);
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetTransform(toB2Vec2(newPosition - box2DToOsgAdjustment), physicsBody->GetAngle());
#else
	_controller->warp(osgbCollision::asBtVector3(newPosition + physicsToModelAdjustment));
	_controller->setVelocityForTimeInterval(btVector3(0,0,0), 0.0);
#endif
}

double Entity::getHeading()
{
	double angle;
	osg::Vec3 axis;
	_transformNode->getAttitude().getRotate(angle, axis);
	if (axis == osg::Vec3(0,0,1))	// make sure the rotation is only around the z-axis.
	{
		return angle;
	}
	else if (axis == osg::Vec3(0,0,-1))
		return -angle;
	logError("Can only obtain heading for entities rotated around z-axis.\n");
	//FIXME: this will not work if the rotation isn't centered on the z-axis.
}
void Entity::setHeading(double angle)
{
	this->setRotation(osg::Quat(angle, osg::Vec3(0,0,1)));
}


StateMachine* Entity::getStateMachine() {
	return _stateMachine;
}

void Entity::changeState(std::string& stateName)
{
	_stateMachine->changeState(stateName);
}
std::string Entity::getCurrentStateName()
{
	return _stateMachine->getStateName();
}
void Entity::returnToPreviousState()
{
	_stateMachine->revertToPreviousState();
}


GameObjectData* Entity::save()
{
	GameObjectData* dataObj = new GameObjectData();

	saveSaveableVariables(dataObj);
	saveGameObjectVariables(dataObj);
	saveEntityVariables(dataObj);

	return dataObj;
}
void Entity::saveEntityVariables(GameObjectData* dataObj)
{
	dataObj->addData("stateMachine", _stateMachine->save());
	dataObj->addData("name", _name);
	dataObj->addData("maxSpeed", _maxSpeed);
	dataObj->addData("capsuleHeight", _capsuleHeight);
	dataObj->addData("capsuleRadius", _capsuleRadius);
}

void Entity::load(GameObjectData* dataObj)
{
	loadSaveableVariables(dataObj);
	loadGameObjectVariables(dataObj);
	loadEntityVariables(dataObj);
}
void Entity::loadEntityVariables(GameObjectData* dataObj)
{
	if(dataObj->hasString("name"))
		_name = dataObj->getString("name");
	if(dataObj->hasFloat("maxSpeed"))
		_maxSpeed = dataObj->getFloat("maxSpeed");
	if(dataObj->getObject("stateMachine"))
		_stateMachine->load(dataObj->getObject("stateMachine"));
	if(dataObj->hasFloat("capsuleHeight"))
		_capsuleHeight = dataObj->getFloat("capsuleHeight");
	if(dataObj->hasFloat("capsuleRadius"))
		_capsuleRadius = dataObj->getFloat("capsuleRadius");
}


void Entity::createController()
{
	if(_controller)
	{
		getCurrentLevel()->getBulletWorld()->removeAction(_controller);
		delete _controller;
	}
	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
#else
		getCurrentLevel()->getBulletWorld()->removeCollisionObject(_physicsBody);
		delete _physicsBody;
#endif
	}


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
	physicsToModelAdjustment = osg::Vec3(0, 0, (_capsuleHeight/2 + _capsuleRadius) * getScale().z());
	btCapsuleShapeZ* shape = new btCapsuleShapeZ(_capsuleRadius, _capsuleHeight);

	btTransform transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(osgbCollision::asBtVector3(getWorldPosition() + physicsToModelAdjustment));
	btVector4 vector4 = osgbCollision::asBtVector4(getWorldRotation().asVec4());
	transform.setRotation(btQuaternion(vector4.x(), vector4.y(), vector4.z(), vector4.w()));
	shape->setLocalScaling(osgbCollision::asBtVector3(getScale()));

	_physicsBody = new btPairCachingGhostObject();
	_physicsBody->setWorldTransform(transform);
	_physicsBody->setCollisionShape(shape);
	_physicsBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	float stepHeight = .35;
	_controller = new ImprovedBulletKinematicCharacterController((btPairCachingGhostObject*)_physicsBody, shape, stepHeight, 2);
	_controller->setGravity(-getCurrentLevel()->getBulletWorld()->getGravity().z());
	getCurrentLevel()->getBulletWorld()->addCollisionObject(_physicsBody, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
	getCurrentLevel()->getBulletWorld()->addAction(_controller);

	_transformNode->setUpdateCallback(new BulletPhysicsNodeCallback(_physicsBody, -physicsToModelAdjustment));

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = _objectType;
	_physicsBody->setUserPointer(userData);
#endif
}

std::string Entity::getName()
{
	return _name;
}


namespace AngelScriptWrapperFunctions
{
	Entity* EntityFactoryFunction()
	{
		return new Entity(root);
	}
}

using namespace AngelScriptWrapperFunctions;


void registerEntity()
{
	static bool registered = false;
	if(registered)
		return;

	registerGameObject();

	getScriptEngine()->registerObjectType("Entity", sizeof(Entity), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("Entity", "Entity@ f()", asFUNCTION(EntityFactoryFunction));

	getScriptEngine()->registerObjectMethod("Entity", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Entity", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	getScriptEngine()->registerObjectMethod("Entity", "void changeState(string &in)", asMETHOD(Entity, changeState), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Entity", "string getStateName()", asMETHOD(Entity, getCurrentStateName), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Entity", "void returnToPreviousState()", asMETHOD(Entity, returnToPreviousState), asCALL_THISCALL);

	registered = true;
}



