#include "Level2D.h"


Level2D::Level2D(std::string mapFilename)
{
	tiledMap = new TiledMap(mapFilename);
	tiledMap->setPosition(osg::Vec3(0.0, 0.0, -5.0));

	gravity = b2Vec2(0.0, -10.0);
	physicsWorld = new b2World(gravity);
	debugDrawer = new DebugDrawer;
	physicsWorld->SetDebugDraw(debugDrawer);

	setCurrentLevel(this);
}


Level2D* currentLevel;

Level2D* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level2D* newLevel) {
	currentLevel = newLevel;
}
