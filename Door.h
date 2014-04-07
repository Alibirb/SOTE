#ifndef DOOR_H
#define DOOR_H

#include "ControlledObject.h"

class btSliderConstraint;

/// Class for a door
class Door : public ControlledObject
{
protected:
	GameObject* _inner;	/// inner component of the door (the part that moves)
	/// the outer part of the door is represented by the nodes and physics body of the Door object.
	btSliderConstraint* _constraint;
public:
	Door();
	Door(GameObjectData* dataObj);
	virtual ~Door();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

	void open();
	void close();

protected:
	void saveDoorVariables(GameObjectData* dataObj);
	void loadDoorVariables(GameObjectData* dataObj);
};

void registerDoor();

#endif // DOOR_H
