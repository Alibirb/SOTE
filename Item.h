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
	osg::ref_ptr<Sprite> sprite;
	osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
	float width = 1.0;
	float height = 1.0;

public:
	Item(std::string imageFilename = DEFAULT_ITEM_IMAGE);
	virtual ~Item();
	Sprite* getSprite();
	osg::PositionAttitudeTransform* getTransformNode();
	virtual void onUpdate(float deltaTime);
protected:
private:
};


/**
 * Callback for the item, used to perform actions that must be performed once per frame.
 */
class ItemUpdateNodeCallback : public osg::NodeCallback
{
private:
	Item* _owner;
public:

	ItemUpdateNodeCallback(Item * owner) {
		this->_owner = owner;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif // ITEM_H
