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

Door::Door() : _inner(NULL)
{
	registerDoor();
}
Door::Door(GameObjectData* dataObj) : Door()
{
	load(dataObj);


	//setPosition(osg::Vec3(0,8,0));
	//setRotation(osg::Quat(0,0,0,1));
	//_inner->setPosition(osg::Vec3(0,-4,0));
/*
	{
		osgbDynamics::MotionState* motionState = (osgbDynamics::MotionState*) ((btRigidBody*) getPhysicsBody())->getMotionState();
		//osg::Matrix* parentTransform = getWorldCoordinates(_inner->getTransformNode());
		//osg::Matrix* parentTransform = getWorldCoordinates(_transformNode);
		//motionState->setParentTransform(*parentTransform);
		//osg::Matrix parentTransform = osg::Matrix::identity();
		//osg::Matrix parentTransform = osg::Matrix::translate(osg::Vec3(0,8,0));
		osg::Matrix parentTransform = osg::Matrix::translate(osg::Vec3(0,8,0));
		motionState->setParentTransform(parentTransform);
		motionState->setTransform(_transformNode.get());
	//	btTransform transform = btTransform();
	//	transform.setIdentity();
		//transform.setOrigin(osgbCollision::asBtVector3(getWorldPosition() + physicsToModelAdjustment));
		//btVector4 vector4 = osgbCollision::asBtVector4(getWorldRotation().asVec4());
		//transform.setRotation(btQuaternion(vector4.x(), vector4.y(), vector4.z(), vector4.w()));
		//transform.setOrigin(btVector3(8,-8,8));
	//	motionState->setWorldTransform(transform);
	}*/

/*
	{
		osgbDynamics::MotionState* motionState = (osgbDynamics::MotionState*) ((btRigidBody*) _inner->getPhysicsBody())->getMotionState();
		//osg::Matrix* parentTransform = getWorldCoordinates(_inner->getTransformNode());
		//osg::Matrix* parentTransform = getWorldCoordinates(_transformNode);
		//motionState->setParentTransform(*parentTransform);
		osg::Matrix parentTransform = osg::Matrix::translate(osg::Vec3(0,8,0));
		motionState->setParentTransform(parentTransform);
		motionState->setTransform(_inner->getTransformNode().get());
		btTransform worldTransform;
        worldTransform.setIdentity();
        worldTransform.setOrigin(btVector3(0,8,0));
		motionState->setWorldTransform(worldTransform);
	}*/

	//_inner->setPosition(osg::Vec3(0,0,0));

	//getPhysicsBody()->setActivationState(DISABLE_DEACTIVATION);
	//_inner->getPhysicsBody()->setActivationState(DISABLE_DEACTIVATION);
	//std::cout << getPhysicsBody()->getCollisionFlags() << std::endl;
	//getPhysicsBody()->setCollisionFlags(0);
	//getPhysicsBody()->set
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

/*
	{
		btTransform testTransform;
		testTransform.setIdentity();
		testTransform.setOrigin(btVector3(8, 0, 160));
		btDefaultMotionState* testMotionState = new btDefaultMotionState(testTransform);
		btRigidBody* testBody = new btRigidBody(10, testMotionState, new btBoxShape(btVector3(2, 2, 2)));
		getCurrentLevel()->getBulletWorld()->addRigidBody(testBody);

	}*/


}

Door::~Door()
{
	//dtor
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
	_inner = new GameObject(dataObj->getObject("innerPart"));
	//_inner = new GameObject();
	//_inner->parentTo(_transformNode);
	//_inner->load(dataObj->getObject("innerPart"));
	//_inner->setPosition(osg::Vec3(0,0,0));
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
		return new Door();
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


