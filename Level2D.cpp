#include "Level2D.h"


Level2D::Level2D(std::string mapFilename)
{
	setCurrentLevel(this);

	//gravity = b2Vec2(0.0, -10.0);
	gravity = b2Vec2(0.0, 0.0);
	physicsWorld = new b2World(gravity);
	debugDrawer = new DebugDrawer();
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_centerOfMassBit;
	debugDrawer->SetFlags(flags);
	physicsWorld->SetDebugDraw(debugDrawer);
	physicsWorld->SetContactListener(new PhysicsContactListener());

	tiledMap = new TiledMap(mapFilename);
	tiledMap->setPosition(osg::Vec3(0.0, 0.0, -5.0));


}


Level2D* currentLevel;

Level2D* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level2D* newLevel) {
	currentLevel = newLevel;
}
