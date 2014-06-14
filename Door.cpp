#include "Door.h"

#include "GameObjectData.h"

#include "AngelScriptEngine.h"

#ifdef USE_BOX2D_PHYSICS
	#include "Box2D/Box2D.h"
	#include "Box2DIntegration.h"
	#include "Box2DDebugDrawer.h"
	#include "PhysicsContactListener.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "osgbCollision/GLDebugDrawer.h"
	#include "osgbCollision/CollisionShapes.h"
	#include "osgbDynamics/MotionState.h"
#endif
#include "PhysicsData.h"
#include "PhysicsNodeCallback.h"

#include "Level.h"

Door::Door(osg::Group* parentNode) : ControlledObject(parentNode), _inner(NULL)
{
	registerDoor();
	_objectType = "Door";
}
Door::Door(GameObjectData* dataObj, osg::Group* parentNode) : Door(parentNode)
{
	load(dataObj);

	btTransform frameInA = btTransform::getIdentity();
	btTransform frameInB = btTransform::getIdentity();
	frameInA.setOrigin(btVector3(0, 1,0));
	frameInB.setOrigin(btVector3(0, 0,0));
	frameInA.getBasis().setEulerZYX(0, pi, pi/2);	/// Used to set the axis it slides along. Seriously, Bullet-demo-writer-guys, you NEED to explain this shit. Anyway, this sets the rotation of the coordinate plane.
	frameInB.getBasis().setEulerZYX(0, pi, pi/2);

	_constraint = new btSliderConstraint(*(btRigidBody*)getPhysicsBody(), *(btRigidBody*)_inner->getPhysicsBody(), frameInA, frameInB, true);
	_constraint->setLowerLinLimit(1);
	_constraint->setUpperLinLimit(2.0f);
	_constraint->setPoweredLinMotor(false);
	_constraint->setMaxLinMotorForce(10000000.0f);
	_constraint->setLowerAngLimit(0);
	_constraint->setUpperAngLimit(0);
	getCurrentLevel()->getBulletWorld()->addConstraint(_constraint, true);
	_constraint->setDbgDrawSize(btScalar(15.0f));

	_stateMachine->changeState(_stateMachine->getStateName());	/// Dirty hack to ensure the door is properly positioned (must call the onEnter() function of the original state)

}

Door::~Door()
{
	getCurrentLevel()->getBulletWorld()->removeConstraint(_constraint);
	markForRemoval(_inner, _inner->_objectType);
}


GameObjectData* Door::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);
	saveControlledObjectVariables(dataObj);
	saveDoorVariables(dataObj);

	return dataObj;
}
void Door::saveDoorVariables(GameObjectData* dataObj)
{
	dataObj->addData("innerPart", _inner);
}

void Door::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadControlledObjectVariables(dataObj);
	loadDoorVariables(dataObj);
}
void Door::loadDoorVariables(GameObjectData* dataObj)
{
	//_inner = new GameObject(dataObj->getObject("innerPart"), root);
	_inner = GameObject::create(dataObj->getObject("innerPart"), root);
}

void Door::open()
{
	//_constraint->setMaxLinMotorForce(100000);
	_constraint->setTargetLinMotorVelocity(100);
	_constraint->setPoweredLinMotor(true);
	_inner->getPhysicsBody()->activate();
}
void Door::close()
{
	//_constraint->setMaxLinMotorForce(100000);
	_constraint->setTargetLinMotorVelocity(-100);
	_constraint->setPoweredLinMotor(true);
	_inner->getPhysicsBody()->activate();
}


namespace AngelScriptWrapperFunctions
{
	Door* DoorFactoryFunction()
	{
		return new Door(root);
	}
}

using namespace AngelScriptWrapperFunctions;


void registerDoor()
{
	static bool registered = false;
	if(registered)
		return;

	registerControlledObject();

	getScriptEngine()->registerObjectType("Door", sizeof(Door), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("Door", "Door@ f()", asFUNCTION(DoorFactoryFunction));

	getScriptEngine()->registerObjectMethod("Door", "void changeState(string &in)", asMETHOD(ControlledObject, changeState), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Door", "string getStateName()", asMETHOD(ControlledObject, getCurrentStateName), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Door", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Door", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	getScriptEngine()->registerObjectMethod("Door", "void open()", asMETHOD(Door, open), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Door", "void close()", asMETHOD(Door, close), asCALL_THISCALL);

	registered = true;
}


