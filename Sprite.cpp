#include "Sprite.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>



/// create quad of the specified size
osg::Geometry* Sprite::createSquare(const osg::Vec3& corner, double width, double height, osg::Image* image)
{
	// set up the Geometry.
	osg::Geometry* geom = new osg::Geometry();

	osg::Vec3Array* coords = new osg::Vec3Array(4);
	(*coords)[0] = corner;
	(*coords)[1] = corner+osg::Vec3(width, 0, 0);
	(*coords)[2] = corner+osg::Vec3(width, height, 0);
	(*coords)[3] = corner+osg::Vec3(0, height, 0);


	geom->setVertexArray(coords);

	osg::Vec3Array* norms = new osg::Vec3Array(1);
	// (*norms)[0] = width^height;
	(*norms)[0] = osg::Vec3(0, 0, 1);
	(*norms)[0].normalize();

	geom->setNormalArray(norms, osg::Array::BIND_OVERALL);

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
	(*tcoords)[0].set(0.0f,0.0f);
	(*tcoords)[1].set(1.0f,0.0f);
	(*tcoords)[2].set(1.0f,1.0f);
	(*tcoords)[3].set(0.0f,1.0f);
	geom->setTexCoordArray(0,tcoords);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

	if (image)
	{
		osg::StateSet* stateset = new osg::StateSet;
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		geom->setStateSet(stateset);
	}

	return geom;
}


Sprite::Sprite(std::string imageName, float width, float height)
{
	osg::Vec3 position = osg::Vec3(0, 0, 0);
	this->width = width;
	this->height = height;
	this->addDrawable(createSquare(position, width, height, osgDB::readImageFile(imageName)));
	setTransparent(true);
}

void Sprite::setOffset(osg::Vec3 offset)
{
	//TODO
}

void Sprite::setImage(std::string imageFilename)
{
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(imageFilename));
	getGeometry()->getStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
}

/// Set whether the sprite should be transparent.
void Sprite::setTransparent(bool transparent)
{
	if(transparent)
	{
		this->getGeometry()->getStateSet()->setRenderBinDetails(1, "transparent");	// Use a different rendering bin that gets rendered after the default one
		this->getGeometry()->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);	// Tell OpenGL that this is a transparent bin, and thus everything should be rendered back-to-front
	}
	// TODO: reverse this to set sprite as opaque.
}
