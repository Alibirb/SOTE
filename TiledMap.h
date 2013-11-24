#ifndef TILEDMAP_H
#define TILEDMAP_H

#include "TmxParser/Tmx.h"
#include "globals.h"
#include "osg/Geometry"


/*
Bugs:
	Code does not account for maps with multiple tilesets
*/

/// Class for a Tiled map. Essentially a wrapper between OSG and TmxParser.
class TiledMap
{
private:
	Tmx::Map *mapData;
	osg::Geode *geode;
	osg::StateSet* state;
	osg::PositionAttitudeTransform *transformNode;
	osg::Geometry* createTile(const osg::Vec3& corner, float width, float height, int gid);
	osg::Vec2Array* getTextureCoordinates(int gid);
public:
	TiledMap(std::string mapFilename);
	virtual ~TiledMap();
	void setPosition(osg::Vec3 position);
	osg::Vec3 getPosition();
protected:
private:
};

#endif // TILEDMAP_H
