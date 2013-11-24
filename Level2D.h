#ifndef LEVEL2D_H
#define LEVEL2D_H


#include "TmxParser/Tmx.h"
#include "Box2D/Box2D.h"


#include "Box2DIntegration.h"
#include "DebugDrawer.h"
#include "TiledMap.h"



/// Class for a 2D level.
class Level2D
{
private:
	b2World *physicsWorld;
	b2Vec2 gravity;
	DebugDrawer *debugDrawer;
	TiledMap *tiledMap;

public:
	Level2D(std::string mapFilename);

	b2World* getPhysicsWorld() {
		return physicsWorld;
	}

protected:
private:

};

Level2D* getCurrentLevel();
void setCurrentLevel(Level2D* newLevel);

#endif // LEVEL2D_H
