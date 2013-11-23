#include "Box2DIntegration.h"


osg::Vec2 b2ToOsgVector(b2Vec2 b2Vector)
{
	return osg::Vec2(b2Vector.x, b2Vector.y);
}
