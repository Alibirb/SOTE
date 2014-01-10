#include "Level.h"

#include "tinyxml/tinyxml.h"

#include "globals.h"

#include "Player.h"
#include "Enemy.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "osgbCollision/CollisionShapes.h"

#include "PhysicsData.h"



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
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
	_physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	_physicsWorld->setGravity(btVector3(0, 0, -9.81));
	_debugDrawer = new osgbCollision::GLDebugDrawer();
	_physicsWorld->setDebugDrawer(_debugDrawer);
	_debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	addToSceneGraph(_debugDrawer->getSceneGraph());
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

	TiXmlDocument doc(filename);
	//bool loadOkay = doc.LoadFile();
	bool loadOkay = doc.LoadFile(file);
	if (!loadOkay)
	{
		logError("Failed to load level file. " + filename);
		logError(doc.ErrorDesc());
	}


	TiXmlHandle docHandle(&doc);
	TiXmlElement* currentElement;
	TiXmlHandle rootHandle = TiXmlHandle(docHandle.FirstChildElement().Element());

	currentElement = rootHandle.FirstChildElement().Element();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			addNode(osgDB::readNodeFile(currentElement->Attribute("source")));
		else if(elementType == "enemy")
		{
			std::string type = currentElement->Attribute("name");
			float x, y, z;
			currentElement->QueryFloatAttribute("x", &x);
			currentElement->QueryFloatAttribute("y", &y);
			currentElement->QueryFloatAttribute("z", &z);

			new Enemy(type, osg::Vec3(x, y, z));
		}
		else if(elementType == "player")
		{
			std::string name = currentElement->Attribute("name");
			float x, y, z;
			currentElement->QueryFloatAttribute("x", &x);
			currentElement->QueryFloatAttribute("y", &y);
			currentElement->QueryFloatAttribute("z", &z);

			addNewPlayer(name, osg::Vec3(x, y, z));
			setActivePlayer(name);
		}
		//else
		//	logWarning("Could not load element of type \"" + currentElement->Value() + "\"");

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

Level* currentLevel;

Level* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level* newLevel) {
	currentLevel = newLevel;
}
