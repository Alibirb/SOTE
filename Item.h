#ifndef ITEM_H
#define ITEM_H

#include "Sprite.h"
#include "globals.h"

#include <osg/PositionAttitudeTransform>

//#define DEFAULT_ITEM_IMAGE "test.png"

/// Class for items.
class Item
{
protected:
	osg::ref_ptr<Sprite> sprite;
	osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
	float width = 1.0;
	float height = 1.0;

public:
	Item(){}
	Item(std::string imageFilename);
	virtual ~Item();
	Sprite* getSprite();
	osg::PositionAttitudeTransform* getTransformNode();
	virtual void onUpdate(float deltaTime);
protected:
private:
};




#endif // ITEM_H
