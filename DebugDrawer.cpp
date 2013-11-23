#include "DebugDrawer.h"

DebugDrawer::DebugDrawer()
{
	//ctor
}

DebugDrawer::~DebugDrawer()
{
	//dtor
}

/// Draw a closed polygon provided in CCW order.
void DebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	//TODO:
}

/// Draw a solid closed polygon provided in CCW order.
void DebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	//TODO:
}

/// Draw a circle.
void DebugDrawer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	//TODO:
}

/// Draw a solid circle.
void DebugDrawer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	//TODO:
}

/// Draw a line segment.
void DebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	//TODO:
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void DebugDrawer::DrawTransform(const b2Transform& xf)
{
	//TODO:
}
