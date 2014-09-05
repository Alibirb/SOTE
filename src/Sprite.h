#ifndef SPRITE_H
#define SPRITE_H

#include <osg/Geode>
#include <osg/Geometry>


#define DEFAULT_SPRITE_IMAGE "test.png"

/**
 * Class for a sprite.
 * Extends osg::Geode so it can be used in the same manner as a 3D model node.
 */
class Sprite : public osg::Geode
{
private:
	osg::Geometry* createSquare(const osg::Vec3& corner, double width, double height, osg::Image* image=NULL);
	float width;
	float height;

public:

	Sprite(std::string imageName = DEFAULT_SPRITE_IMAGE, float width = 1.0, float height = 1.0);


	/// Sets the offset of the bottom left corner of the sprite.
	void setOffset(osg::Vec3 offset);

	osg::Geometry* getGeometry() {
		return this->getDrawable(0)->asGeometry();
	}

	void setImage(std::string imageFilename);

	/// Set whether the sprite should be transparent.
	void setTransparent(bool transparent);

	float getWidth() {
		return width;
	}
	float getHeight() {
		return height;
	}
};

#endif // SPRITE_H
