#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"

#include "Sprite.h"
#include "globals.h"

#include <osg/PositionAttitudeTransform>

/// Class for items.
class Item : public GameObject
{
protected:
	float width = 1.0;
	float height = 1.0;
public:
	Item(){}
	Item(std::string imageFilename);
	virtual ~Item();
	Sprite* getSprite();
	virtual void onUpdate(float deltaTime);
	virtual void onCollision(GameObject* other);
};




#endif // ITEM_H
