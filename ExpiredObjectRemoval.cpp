#include "ExpiredObjectRemoval.h"

#include <deque>
//#include <iostream>

#include "Enemy.h"
#include "Projectile.h"





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
		if (objectType == "Enemy")
			delete ((Enemy*) expiredObjects.front().object);
		else if (objectType == "Entity")
			delete ((Entity*) expiredObjects.front().object);
		else if (objectType == "Item")
			delete ((Item*) expiredObjects.front().object);
		else if (objectType == "Projectile")
			delete ((Projectile*) expiredObjects.front().object);
		else
			logError("Object of unfamiliar type \"" + objectType + "\" marked for deletion");	// cannot properly delete the object unless we know what type it is.

		expiredObjects.pop_front();
	}
}

void markForRemoval(void *toBeEXTERMINATED, std::string objectType) {
	ObjectInfo info;
	info.object = toBeEXTERMINATED;
	info.objectType = objectType;
	expiredObjects.push_back(info);
}
