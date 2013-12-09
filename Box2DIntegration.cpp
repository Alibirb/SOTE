#include "Box2DIntegration.h"


osg::Vec2 b2Vec2ToOsgVec2(b2Vec2 b2Vector)
{
	return osg::Vec2(b2Vector.x, b2Vector.y);
}

osg::Vec3 b2Vec2ToOsgVec3(b2Vec2 b2Vector, double zCoordinate)
{
	return osg::Vec3(b2Vector.x, b2Vector.y, zCoordinate);
}

b2Vec2 toB2Vec2(osg::Vec3 osgVector)
{
	return b2Vec2(osgVector.x(), osgVector.y());
}

osg::Vec4 b2ColorToOsgVec4(b2Color color, float alpha)
{
	return osg::Vec4(color.r, color.g, color.b, alpha);
}
