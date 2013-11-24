#ifndef SPRITE_H
#define SPRITE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>

/**
 * Class for a sprite.
 * Extends osg::Geode so it can be used in the same manner as a 3D model node.
 */
class Sprite : public osg::Geode
{
private:
	osg::Geometry* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL);
	float width;
	float height;
	osg::Vec3 position;

public:

	Sprite(std::string imageName = "test.png", float x = 0.0, float y = 0.0, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->addDrawable(createSquare( position, osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f,1.0f,0.0f), osgDB::readImageFile(imageName)));
	}

	Sprite(osg::Image* image, float x, float y, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->addDrawable(createSquare( position, osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f,1.0f,0.0f), image));
	}

	Sprite(osg::Geometry* geometry, float x, float y, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->addDrawable(geometry);
	}

	osg::Geometry* getGeometry() {
		return this->getDrawable(0)->asGeometry();
	}
};

#endif // SPRITE_H
