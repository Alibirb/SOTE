#include "Level.h"

#include "tinyxml/tinyxml2.h"

#include "globals.h"

#include "Player.h"
#include "ControlledObject.h"
#include "Controller.h"
#include "Door.h"
#include "DangerZone.h"

#include "PhysicsData.h"
#include "GameObjectData.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "osgbCollision/CollisionShapes.h"


#include "yaml-cpp/yaml.h"




void myTickCallback(btDynamicsWorld* world, btScalar timeStep)
{

	for(auto kv : getPlayers())
		kv.second->setObjectToInteractWith(NULL);	/// reset each Player's stored interable object, since we're going to regenerate it.

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();


		PhysicsUserData* dataA = (PhysicsUserData*) obA->getUserPointer();
		PhysicsUserData* dataB = (PhysicsUserData*) obB->getUserPointer();

		if(!dataA || !dataB)
		{
			logError("Physics object with no userdata.");
			continue;
		}

		/*if(dataA->ownerType == "Projectile" && (dataB->ownerType == "Fighter" || dataB->ownerType == "Player") )
		{
			((Fighter*)dataB->owner)->onCollision((Projectile*)dataA->owner);
			((Projectile*)dataA->owner)->onCollision((Fighter*)dataB->owner);
		}
		else if((dataA->ownerType == "Fighter" || dataA->ownerType == "Player") && dataB->ownerType == "Projectile")
		{
			((Fighter*)dataA->owner)->onCollision((Projectile*)dataB->owner);
			((Projectile*)dataB->owner)->onCollision((Fighter*)dataA->owner);
		}*/
		if(dataA->ownerType == "DangerZone" && (dataB->ownerType == "Fighter" || dataB->ownerType == "Player") )
		{
			((Fighter*)dataB->owner)->onCollision((DangerZone*)dataA->owner);
			//((DangerZone*)dataA->owner)->onCollision((Fighter*)dataB->owner);
		}
		else if((dataA->ownerType == "Fighter" || dataA->ownerType == "Player") && dataB->ownerType == "DangerZone")
		{
			((Fighter*)dataA->owner)->onCollision((DangerZone*)dataB->owner);
			//((DangerZone*)dataB->owner)->onCollision((Fighter*)dataA->owner);
		}
		else if(dataA->ownerType == "Player" && dataB->ownerType == "Controller")
		{
			((Player*)dataA->owner)->onCollision((Controller*)dataB->owner);
			//((GameObject*)dataB->owner)->onCollision((GameObject*)dataA->owner);
		}
		else if(dataA->ownerType == "Controller" && dataB->ownerType == "Player")
		{
			((Player*)dataB->owner)->onCollision((Controller*)dataA->owner);
			//((GameObject*)dataB->owner)->onCollision((GameObject*)dataA->owner);
		}
		else if(dataA->ownerType != "Level" && dataB->ownerType != "Level")		// Neither body is part of the Level geometry
		{
			((GameObject*)dataA->owner)->onCollision((GameObject*)dataB->owner);
			((GameObject*)dataB->owner)->onCollision((GameObject*)dataA->owner);
		}

		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;


			}
		}
	}
}


Level::Level(std::string filename)
{
	setCurrentLevel(this);

#ifdef USE_BOX2D_PHYSICS
	gravity = b2Vec2(0.0, 0.0);
	physicsWorld = new b2World(gravity);
	debugDrawer = new Box2DDebugDrawer();
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_centerOfMassBit;
	debugDrawer->SetFlags(flags);
	physicsWorld->SetDebugDraw(debugDrawer);
	physicsWorld->SetContactListener(new PhysicsContactListener());
#else
	// Bullet world setup
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btBroadphaseInterface *broadphase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
	_physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	_physicsWorld->setGravity(btVector3(0, 0, -9.81));
	_debugDrawer = new osgbCollision::GLDebugDrawer();
	_physicsWorld->setDebugDrawer(_debugDrawer);
	_debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	addToSceneGraph(_debugDrawer->getSceneGraph());
	_physicsWorld->setInternalTickCallback(myTickCallback);
#endif

	_filename = filename;
	load(filename);
}

Level::~Level()
{
}


#ifndef USE_TILEMAP
void Level::addNode(osg::Node* node)
{
	if(!_levelGeometry)
	{
		_levelGeometry = node;
		addToSceneGraph(node);

#ifndef USE_BOX2D_PHYSICS
		btTriangleMeshShape* levelShape = osgbCollision::btTriMeshCollisionShapeFromOSG(_levelGeometry);
		_levelBody = new btRigidBody(0, new btDefaultMotionState(), levelShape);
		_physicsWorld->addRigidBody(_levelBody);
#endif

		PhysicsUserData *userData = new PhysicsUserData;
		userData->owner = this;
		userData->ownerType = "Level";
#ifdef USE_BOX2D_PHYSICS
		levelBody->SetUserData(userData);
#else
		_levelBody->setUserPointer(userData);
#endif
	}
}
#endif

GameObjectData* Level::save()
{
	GameObjectData* data = new GameObjectData("Level");

	//for(GameObject* object : _objects)
	//	data->addChild(object);
//	data->addData("children", _objects);
	std::vector<GameObject*> objectVector;

	for(GameObject* object : _objects)
		objectVector.push_back(object);

	data->addData("children", objectVector);

	return data;
}


void Level::saveAsYaml(std::string filename)
{
	YAML::Emitter emitter;
	//emitter << this->save()->toYAML();
	this->save()->toYAML(emitter);
	//emitter << YAML::Flow << this->save()->toYAML();
	std::ofstream fout(filename);
	fout << emitter.c_str();
	//fout << this->save()->toYAML();
}
void Level::loadFromYaml(std::string filename)
{
	GameObjectData* data = new GameObjectData(YAML::LoadFile(filename));

	for(auto child : data->getObjectList("children"))
	//for(auto child : data->getChildren())
	{
		std::string elementType = child->getType();
		if(elementType == "GameObject")
		{
			addObject(new GameObject(child));
		}
		else if(elementType == "ControlledObject")
		{
			addObject(new ControlledObject(child));
		}
		else if(elementType == "Door")
			addObject(new Door(child));
		else if(elementType == "Controller")
		{
			addObject(new Controller(child));
		}
		else if(elementType == "Fighter")
		{
			addObject(new Fighter(child));
		}
		else if(elementType == "Player")
		{
			std::string name = child->getString("name");
			addPlayer(name, new Player(child));
			setActivePlayer(name);
			addObject(getActivePlayer());
		}
		else if(elementType == "DangerZone")
			addObject(new DangerZone(child));
		else
		{
			std::string warning = "";
			warning += "Could not load element of type \"";
			warning += elementType;
			warning += "\".";
			logWarning(warning);
		}


	}
}
void Level::load(std::string filename)
{
	loadFromYaml(filename);
}
void Level::reload(std::string filename)
{
	std::cout << "reloading" << std::endl;

	for(auto obj : _objects)
		markForRemoval(obj, obj->_objectType);
	loadFromYaml(filename);
}
void Level::reload()
{
	reload(_filename);
}

void Level::addObject(GameObject* obj)
{
	_objects.push_back(obj);
}
void Level::removeObject(GameObject* obj)
{
	_objects.remove(obj);
}



Level* currentLevel;

Level* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level* newLevel) {
	currentLevel = newLevel;
}
