#ifndef ITEM_H
#define ITEM_H

#include "Sprite.h"
#include "globals.h"

#include <osg/PositionAttitudeTransform>

#define DEFAULT_ITEM_IMAGE "test.png"

/// Class for items.
class Item
{
protected:
	Sprite *sprite;
	osg::PositionAttitudeTransform *transformNode;
	float width = 1.0;
	float height = 1.0;
public:
	Item(std::string imageFilename = DEFAULT_ITEM_IMAGE);
	virtual ~Item();
	Sprite* getSprite();
	osg::PositionAttitudeTransform* getTransformNode();
protected:
private:
};

#endif // ITEM_H
