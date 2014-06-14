#include "ExpiredObjectRemoval.h"

#include <deque>
//#include <iostream>



/// Make sure to include headers for all objects we might delete.
#include "Fighter.h"
#include "Projectile.h"
#include "Player.h"
#include "Controller.h"
#include "TemporaryText.h"
#include "DangerZone.h"
#include "Door.h"
#include "Light.h"

#include "globals.h"

struct ObjectInfo
{
	void* object;
	std::string objectType;
};

std::deque<ObjectInfo> expiredObjects;


void removeExpiredObjects()
{
	while (expiredObjects.size() > 0)
	{
		// For each object in the list, cast it to the appropriate type and then delete it.
		std::string objectType = expiredObjects.front().objectType;


		#define EOR_OBJECT_TYPE_ELSE_IF(type) else if (objectType == #type) delete ((type*) expiredObjects.front().object);

		if (objectType == "Fighter")
			delete ((Fighter*) expiredObjects.front().object);
		else if (objectType == "Entity")
			delete ((Entity*) expiredObjects.front().object);
		else if (objectType == "Player")
			delete ((Player*) expiredObjects.front().object);
		else if (objectType == "GameObject")
			delete ((GameObject*) expiredObjects.front().object);
		else if (objectType == "Controller")
			delete ((Controller*) expiredObjects.front().object);
		else if (objectType == "Projectile")
			delete ((Projectile*) expiredObjects.front().object);
		else if (objectType == "TemporaryText")
			delete ((TemporaryText*) expiredObjects.front().object);
		else if (objectType == "DangerZone")
			delete ((DangerZone*) expiredObjects.front().object);
		else if (objectType == "Light")
			delete ((Light*) expiredObjects.front().object);
		//EOR_OBJECT_TYPE_ELSE_IF(Door)
		//EOR_OBJECT_TYPE_ELSE_IF(ControlledObject)
		else if (objectType == "Door")
			delete ((Door*) expiredObjects.front().object);
		else if (objectType == "ControlledObject")
			delete ((ControlledObject*) expiredObjects.front().object);

		else
			logError("Object of unfamiliar type \"" + objectType + "\" marked for deletion");	/// cannot properly delete the object unless we know what type it is.

		expiredObjects.pop_front();
	}
}

bool isMarked(void* object)
{
	for(ObjectInfo info : expiredObjects)
	{
		if(info.object == object)
			return true;
	}
	return false;
}

void markForRemoval(void *toBeEXTERMINATED, std::string objectType)
{
	if(isMarked(toBeEXTERMINATED))
	{
		logWarning("Object already marked for removal.");
		return;
	}
	ObjectInfo info;
	info.object = toBeEXTERMINATED;
	info.objectType = objectType;
	expiredObjects.push_back(info);
}
void markForRemoval(Saveable* toBeEXTERMINATED)
{
	markForRemoval(toBeEXTERMINATED, toBeEXTERMINATED->getType());
}

