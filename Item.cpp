#include "Item.h"

#include "OwnerUpdateCallback.h"

Item::Item(std::string imageFilename)
{

	sprite = new Sprite(imageFilename);
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(sprite);
	sprite->setUpdateCallback(new OwnerUpdateCallback<Item>(this));
}

Item::~Item()
{
	if(transformNode->getNumParents() == 0)
		logError("Item with no parents found.");
	if(transformNode->getNumParents() > 1)
		logError("Item with multiple parents found.");
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

void Item::onUpdate(float deltaTime)
{

}

