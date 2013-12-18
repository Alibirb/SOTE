#include "Item.h"

Item::Item(std::string imageFilename)
{
	sprite = new Sprite(imageFilename);
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(sprite);
}

Item::~Item()
{
	transformNode->getParent(0)->removeChild(transformNode);	// remove the node from the scenegraph.
}

Sprite* Item::getSprite()
{
	return sprite;
}

osg::PositionAttitudeTransform* Item::getTransformNode()
{
	return transformNode;
}
