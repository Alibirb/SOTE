#include "Item.h"

#include "OwnerUpdateCallback.h"

Item::Item(std::string imageFilename)
{
	_useSpriteAsModel = true;
	loadModel(imageFilename);
}

Item::~Item()
{
}

Sprite* Item::getSprite()
{
	return dynamic_pointer_cast<Sprite>(_modelNode);
}

void Item::onUpdate(float deltaTime)
{

}

void Item::onCollision(GameObject* other)
{
	// TODO
}

