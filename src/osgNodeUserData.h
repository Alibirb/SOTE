#ifndef OSGNODEUSERDATA_H_INCLUDED
#define OSGNODEUSERDATA_H_INCLUDED


#include <osg/Referenced>

#include "GameObject.h"

/// User data object to give OSG nodes a link to the SOTE object that owns them.
class osgNodeUserData : public osg::Referenced
{
public:
	GameObject* owner;

public:
	osgNodeUserData(GameObject* owner)
	{
		this->owner = owner;
	}

};


#endif // OSGNODEUSERDATA_H_INCLUDED
