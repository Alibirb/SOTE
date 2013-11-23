#ifndef ITEM_H
#define ITEM_H

#include "Sprite.h"
#include "globals.h"

/// Class for items.
class Item
{
protected:
	Sprite *sprite;
	osg::PositionAttitudeTransform *transformNode;
public:
	Item();
	virtual ~Item();
protected:
private:
};

#endif // ITEM_H
