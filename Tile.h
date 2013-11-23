#ifndef TILE_H
#define TILE_H

#include "globals.h"

/**
 *
 */
class Tile
{
private:
	osg::Geometry* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL);
	float width;
	float height;
	osg::Vec3 position;

public:
	osg::Geometry* geometry;

	Tile(osg::Geometry* geometry, float x, float y, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->geometry = geometry;
	}

	Tile(osg::Image* image, float x, float y, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->geometry = createSquare( position, osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f,1.0f,0.0f), image);
	}

	Tile(std::string imageName = "test.png", float x = 0.0, float y = 0.0, float z = 0.0)
	{
		position = osg::Vec3(x, y, z);
		width = 1.0;
		height = 1.0;
		this->geometry = createSquare( position, osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f,1.0f,0.0f), osgDB::readImageFile(imageName));
	}

	void setPosition(int x, int y, int z = 0)
	{
		position.set(x, y, z);
		osg::Vec3Array* coords = new osg::Vec3Array(4);
		(*coords)[0] = position;
		(*coords)[1] = position+osg::Vec3(width, 0.0, 0.0);
		(*coords)[2] = position+osg::Vec3(width, height, 0.0);
		(*coords)[3] = position+osg::Vec3(0.0, height, 0.0);

		this->geometry->setVertexArray(coords);
	}

	void setSize(int width, int height)
	{
		this->width = width;
		this->height = height;
		setPosition(position.x(), position.y(), position.z());	// Resize the actual geometry.
	}

};

#endif // TILE_H
