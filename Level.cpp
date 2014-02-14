#include "Level.h"

#include "tinyxml/tinyxml2.h"

#include "globals.h"

#include "Player.h"
#include "Enemy.h"
#include "ControlledObject.h"
#include "Controller.h"
#include "PhysicsData.h"
#include "GameObjectData.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "osgbCollision/CollisionShapes.h"





void myTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
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

		if(dataA->ownerType == "Projectile" && (dataB->ownerType == "Enemy" || dataB->ownerType == "Player") )
		{
			((Fighter*)dataB->owner)->onCollision((Projectile*)dataA->owner);
			((Projectile*)dataA->owner)->onCollision((Fighter*)dataB->owner);
		}
		else if((dataA->ownerType == "Enemy" || dataA->ownerType == "Player") && dataB->ownerType == "Projectile")
		{
			((Fighter*)dataA->owner)->onCollision((Projectile*)dataB->owner);
			((Projectile*)dataB->owner)->onCollision((Fighter*)dataA->owner);
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

	loadFromXml(filename);
}

Level::~Level()
{
}

void Level::loadFromXml(std::string filename)
{
#ifdef USE_TILEMAP
	tiledMap = new TiledMap(mapFilename);
	tiledMap->setPosition(osg::Vec3(0.0, 0.0, -5.0));
#else

	FILE *file = fopen(filename.c_str(), "rb");
	if(!file)
		logError("Failed to open file " + filename);

	XMLDocument doc(filename.c_str());
	if (doc.LoadFile(file)  != tinyxml2::XML_NO_ERROR)
	{
		logError("Failed to load level file. " + filename);
		logError(doc.GetErrorStr1());
	}


	XMLHandle docHandle(&doc);
	XMLElement* currentElement;
	XMLHandle rootHandle = XMLHandle(docHandle.FirstChildElement().ToElement());

	currentElement = rootHandle.FirstChildElement().ToElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			addNode(osgDB::readNodeFile(currentElement->Attribute("source")));
		else if(elementType == "gameObject")
		{
			addObject(new GameObject(currentElement));
		}
		else if(elementType == "controlledObject")
		{
			addObject(new ControlledObject(currentElement));
		}
		else if(elementType == "controller")
		{
			addObject(new Controller(currentElement));
		}
		else if(elementType == "enemy")
		{
			addObject(new Enemy(currentElement));
		}
		else if(elementType == "player")
		{
			std::string name = currentElement->Attribute("name");
			addPlayer(name, new Player(currentElement));
			setActivePlayer(name);
			addObject(getActivePlayer());
		}
		else
		{
			std::string warning = "";
			warning += "Could not load element of type \"";
			warning += currentElement->Value();
			warning += "\".";
			logWarning(warning);
		}


	}
#endif // USE_TILEMAP
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
	GameObjectData* data = new GameObjectData("level");

	for(GameObject* object : _objects)
		data->addChild(object);

	return data;
}

void Level::saveAsXml(std::string filename)
{
	XMLDocument* doc = new XMLDocument(filename.c_str());
	doc->InsertFirstChild(this->save()->toXML(doc));	// Make this the root element.

	doc->SaveFile(filename.c_str());
}

void Level::addObject(GameObject* obj)
{
	_objects.push_back(obj);
}



Level* currentLevel;

Level* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level* newLevel) {
	currentLevel = newLevel;
}
