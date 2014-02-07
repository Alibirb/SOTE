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


void myTickCallback(btDynamicsWorld *world, btScalar timeStep);

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
	btRigidBody* _levelBody;
#endif

#ifdef USE_TILEMAP
	TiledMap *tiledMap;
#else
	osg::ref_ptr<osg::Node> _levelGeometry;
#endif

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

protected:
	void loadFromXml(std::string filename);
#ifndef USE_TILEMAP
	void addNode(osg::Node* node);
#endif

};

Level* getCurrentLevel();
void setCurrentLevel(Level* newLevel);



#endif // LEVEL_H
