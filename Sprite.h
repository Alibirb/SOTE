#ifndef SPRITE_H
#define SPRITE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>

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

	Sprite(std::string imageName = DEFAULT_SPRITE_IMAGE, float width = 1.0, float height = 1.0)
	{
		osg::Vec3 position = osg::Vec3(0, 0, 0);
		this->width = width;
		this->height = height;
		this->addDrawable(createSquare(position, width, height, osgDB::readImageFile(imageName)));
		setTransparent(true);
	}


	/// Sets the offset of the sprite.
	void setOffset(osg::Vec3 offset)
	{
		//TODO
	}

	osg::Geometry* getGeometry() {
		return this->getDrawable(0)->asGeometry();
	}

	void setImage(std::string imageFilename)
	{
		osg::Texture2D* texture = new osg::Texture2D;
        texture->setImage(osgDB::readImageFile(imageFilename));
		getGeometry()->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	}

	/// Set whether the sprite should be transparent.
	void setTransparent(bool transparent)
	{
		if(transparent)
		{
			this->getGeometry()->getStateSet()->setRenderBinDetails(1, "transparent");	// Use a different rendering bin that gets rendered after the default one
			this->getGeometry()->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);	// Tell OpenGL that this is a transparent bin, and thus everything should be rendered back-to-front
		}
		// TODO: reverse this to set sprite as opaque.
	}
};

#endif // SPRITE_H
