#ifndef LEVEL_H
#define LEVEL_H

#include <string>


#ifdef USE_BOX2D_PHYSICS
	#include "Box2D/Box2D.h"
	#include "Box2DIntegration.h"
	#include "Box2DDebugDrawer.h"
	#include "PhysicsContactListener.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
	#include "osgbCollision/GLDebugDrawer.h"
#endif

#ifdef USE_TILEMAP
	#include "TiledMap.h"
	#include "TmxParser/Tmx.h"
#endif

#include <iostream>
#include <unordered_map>

class GameObject;
class GameObjectData;
class Player;
class BaseCameraManipulator;


void myTickCallback(btDynamicsWorld *world, btScalar timeStep);


/// Class for a game level.
class Level
{
private:
#ifdef USE_BOX2D_PHYSICS
	b2World *physicsWorld;
	b2Vec2 gravity;
	Box2DDebugDrawer *debugDrawer;
#else	// Bullet
	btDiscreteDynamicsWorld* _physicsWorld;
	osgbCollision::GLDebugDrawer* _debugDrawer;
#endif

	std::list<GameObject*> _objects;
	std::unordered_map<std::string, Player*> _players;
	std::list<std::string> _playerNames;
	std::string _activePlayerName;

	osg::Group* _levelRoot;	/// Root node that everything in the Level is parented to.

	std::string _filename;	/// filename used to load the level. Stored to allow reloading.

	osg::ref_ptr<BaseCameraManipulator> _cameraManipulator;


public:
	Level(std::string filename);
	~Level();

#ifdef USE_BOX2D_PHYSICS
	b2World* getPhysicsWorld() {
		return physicsWorld;
	}
	Box2DDebugDrawer* getDebugDrawer() {
		return debugDrawer;
	}
#else	// Bullet
	btDiscreteDynamicsWorld* getBulletWorld() {
		return _physicsWorld;
	}
	osgbCollision::GLDebugDrawer* getDebugDrawer() {
		return _debugDrawer;
	}
#endif

	void updatePhysics(double deltaTime)
	{
#ifdef USE_BOX2D_PHYSICS
		_physicsWorld->Step(deltaTime, 6, 2);
		_debugDrawer->beginDraw();
		_physicsWorld->DrawDebugData();
		_debugDrawer->endDraw();
#else
		_physicsWorld->stepSimulation(deltaTime);
		_debugDrawer->BeginDraw();
		_physicsWorld->debugDrawWorld();
		_debugDrawer->EndDraw();
#endif
	}

	virtual GameObjectData* save();	/// Saves the object's data

	void saveAsFile(std::string filename);
	std::string saveAsString();

	void loadFromFile(std::string filename);
	void loadFromString(std::string text);
	void reload();
	void clear();	/// Removes everything from the Level.

	void removeObject(GameObject* obj);

	Player* getClosestPlayer(osg::Vec3 position);
	Player* getActivePlayer();
	void switchToNextPlayer();
	void switchToPreviousPlayer();
	void setActivePlayer(std::string newActivePlayer);
	void addPlayer(std::string playerName, Player* thePlayer);
	std::unordered_map<std::string, Player*> getPlayers();
	std::list<std::string> getPlayerNames();
	std::string getFilename();
	osg::Group* getRootNode();

	BaseCameraManipulator* getCameraManipulator() { return _cameraManipulator;}
	void setCameraManipulator(BaseCameraManipulator* cameraManipulator);

protected:

	void addObject(GameObject* obj);

};

Level* getCurrentLevel();
void setCurrentLevel(Level* newLevel);



#endif // LEVEL_H
