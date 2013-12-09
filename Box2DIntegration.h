#ifndef BOX2DINTEGRATION_H_
#define BOX2DINTEGRATION_H_

#include "Box2D/Box2D.h"
#include "osg/Vec2"
#include "osg/Vec3"
#include "osg/Vec4"
#include <iostream>

/// Converts a Box2D vector to an OSG Vec2
osg::Vec2 b2Vec2ToOsgVec2(b2Vec2 b2Vector);

/// Converts a Box2D vector to an OSG Vec3 with the given z-coordinate
osg::Vec3 b2Vec2ToOsgVec3(b2Vec2 b2Vector, double zCoordinate);

/// Converts an OSG Vec3 to a Box2D vector.
b2Vec2 toB2Vec2(osg::Vec3 osgVector);

/// Converts a Box2D Color to an OSG Vec4
osg::Vec4 b2ColorToOsgVec4(b2Color color, float alpha = 1.0);

/// userdata to attach to physics bodies so the objects that own them can be altered (e.g. enemies get hurt)
struct Box2DUserData
{
	std::string ownerType;
	void* owner;
};


#endif // BOX2DINTEGRATION_H_
