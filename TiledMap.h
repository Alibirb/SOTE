#ifndef TILEDMAP_H
#define TILEDMAP_H

#include "TmxParser/Tmx.h"
#include "globals.h"
#include "osg/Geometry"
#include <osg/PositionAttitudeTransform>


class b2Body;

/// Class for a Tiled map. Essentially a wrapper between OSG and TmxParser.
class TiledMap
{
private:
	Tmx::Map *mapData;
	osg::Geode *geode;
	osg::StateSet* state;
	osg::PositionAttitudeTransform *transformNode;
	std::vector<b2Body*> physicsBodies;
	float tileWidth;
	float tileHeight;
	//std::vector<Tmx::Object*> objects;

	/// Creates a Geometry for a tile with the given position, size, and gid.
	osg::Geometry* createTile(const osg::Vec3& corner, float width, float height, int gid);

	/// Returns the texture coordinates for the given gid
	osg::Vec2Array* getTextureCoordinates(int gid);

	/// Loads a collision layer
	void loadCollisionLayer(const Tmx::ObjectGroup *objectGroup);

	/// Loads an entity placement layer
	void loadEntityLayer(const Tmx::ObjectGroup *objectGroup);

public:
	TiledMap(std::string mapFilename);
	virtual ~TiledMap();
	void setPosition(osg::Vec3 position);
	osg::Vec3 getPosition();


protected:
private:
};

#endif // TILEDMAP_H
