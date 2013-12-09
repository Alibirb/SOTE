#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "Box2D/Common/b2Draw.h"
#include "Box2DIntegration.h"
#include "osg/Geometry"


class DebugDrawer : public b2Draw
{
private:
	//osg::ref_ptr<osg::Group> _group;
	//osg::ref_ptr<osg::Geode> _geode;
	//osg::ref_ptr<osg::Geometry> _lineGeometry;
	osg::Group* _group;
	osg::Geode* _geode;
	osg::Geometry* _lineGeometry;
	osg::Vec3Array* _lineVertices;
	osg::Vec4Array* _lineColors;

	float drawZCoordinate = 1.0;	// The z-coordinate to place the debug drawing.
	bool active;

public:
	DebugDrawer();
	virtual ~DebugDrawer();

	void beginDraw();
	void endDraw();

	/// Draw a closed polygon provided in CCW order.
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a circle.
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	/// Draw a solid circle.
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	/// Draw a line segment.
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	void DrawTransform(const b2Transform& xf);

protected:
private:
};

#endif // DEBUGDRAWER_H
