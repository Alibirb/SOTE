#ifndef LEVEL2D_H
#define LEVEL2D_H



#include "TmxParser/Tmx.h"
#include "Box2D/Box2D.h"


#include "Tile.h"
#include "Box2DIntegration.h"
#include "DebugDrawer.h"

/*
Bugs:
	Code does not account for maps with multiple tilesets
*/

class Level2D
{
private:
	b2World *physicsWorld;
	b2Vec2 gravity;
	DebugDrawer *debugDrawer;

public:
	int sizeX, sizeY;
	int numLayers;
	std::vector<std::vector<std::vector<Tile*> > > tiles;
	osg::Geode *geode;
	Tmx::Map *tiledMap;
	bool usesTmx;
	osg::StateSet* state;

	Level2D(std::string mapFilename);

	Tile* createTile(const osg::Vec3& corner, float width, float height, int gid);
	osg::Vec2Array* getTextureCoordinates(int gid);

	b2World* getPhysicsWorld() {
		return physicsWorld;
	}

protected:
private:

};

Level2D* getCurrentLevel();
void setCurrentLevel(Level2D* newLevel);

#endif // LEVEL2D_H
