#include "Box2DDebugDrawer.h"
#include "globals.h"

Box2DDebugDrawer::Box2DDebugDrawer()
{
	_group = new osg::Group();
	//_group->setName("Box2D Debug Drawing");

	_geode = new osg::Geode();
	//_geode->setName("Box2D Debug Drawing");
	//_geode->setDataVariance(osg::Object::DYNAMIC);
	/// BUG: Data variance should be set to dynamic, but that seems to drastically increase seg faults in stl_construct. Without setting dynamic data variance, program still seg faults in stl_construct if too many physics objects are created, but otherwise seems to perform fine.


	_lineGeometry = new osg::Geometry();
	//_lineGeometry->setDataVariance(osg::Object::DYNAMIC);
	_lineGeometry->setUseDisplayList(false);
	_lineGeometry->setUseVertexBufferObjects(false);
	_lineVertices = new osg::Vec3Array();
	_lineGeometry->setVertexArray(_lineVertices);
	_lineColors = new osg::Vec4Array();
	_lineGeometry->setColorArray(_lineColors);
	_lineGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	_geode->addDrawable(_lineGeometry);

	_filledPolygonGeometry = new osg::Geometry();
	//_filledPolygonGeometry->setDataVariance(osg::Object::DYNAMIC);
	_filledPolygonGeometry->setUseDisplayList(false);
	_filledPolygonGeometry->setUseVertexBufferObjects(false);
	_filledPolygonVertices = new osg::Vec3Array();
	_filledPolygonGeometry->setVertexArray(_filledPolygonVertices);
	_filledPolygonColors = new osg::Vec4Array();
	_filledPolygonGeometry->setColorArray(_filledPolygonColors);
	_filledPolygonGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	_geode->addDrawable(_filledPolygonGeometry);

	addToSceneGraph(_geode);
	setEnabled(true);

	_drawing = false;
}

Box2DDebugDrawer::~Box2DDebugDrawer()
{
	//dtor
}

void Box2DDebugDrawer::setEnabled(bool enable)
{
	if(!enable)
	{
		beginDraw();
	}
	_enabled = enable;
}
bool Box2DDebugDrawer::getEnabled()
{
	return _enabled;
}


void Box2DDebugDrawer::beginDraw()
{
	if(_lineVertices->size() > 0)
	{
		_lineGeometry->removePrimitiveSet(0);
		_lineVertices->clear();
		_lineColors->clear();
	}

	if(_filledPolygonVertices->size() > 0)
	{
		_filledPolygonGeometry->removePrimitiveSet(0);
		_filledPolygonVertices->clear();
		_filledPolygonColors->clear();
	}

	_drawing = true;
}

void Box2DDebugDrawer::endDraw()
{
	if(_lineVertices->size())
		_lineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, _lineVertices->size()));

	if(_filledPolygonVertices->size())
		//_filledPolygonGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON, 0, _filledPolygonVertices->size()));
		_filledPolygonGeometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, _filledPolygonVertices->size()));

	_drawing = false;
}

/// Draw a closed polygon provided in CCW order.
void Box2DDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	if(!_enabled)
		return;

	for (int i = 0; i < vertexCount; ++i)
	{
		if (i == vertexCount - 1)
			this->DrawSegment(vertices[i], vertices[0], color);	// last point connects to the first one.
		else
			this->DrawSegment(vertices[i], vertices[i+1], color);
	}
}

/// Draw a solid closed polygon provided in CCW order.
/// BUG: Current implementation (when set to draw solid) does not work for non-quads.
void Box2DDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	if(!_enabled)
		return;
#ifdef DO_SOLID_DRAWING
	for (int i = 0; i < vertexCount; ++i)
	{
		_filledPolygonVertices->push_back(b2Vec2ToOsgVec3(vertices[i], drawZCoordinate));
		_filledPolygonColors->push_back(b2ColorToOsgVec4(color));
	}
#else
	DrawPolygon(vertices, vertexCount, color);
#endif
}

/// Draw a circle.
void Box2DDebugDrawer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	if(!_enabled)
		return;

	static int numSegments = 16;
	b2Vec2 vertices[numSegments];

	for(int i = 0; i < numSegments; ++i)
	{
		double angle = 2*pi * (i / (float) numSegments);
		vertices[i] = center + b2Vec2(radius * cos(angle), radius * sin(angle));
	}
	DrawPolygon(vertices, numSegments, color);

}

/// Draw a solid circle.
void Box2DDebugDrawer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
#ifdef DO_SOLID_DRAWING
	getDebugDisplayer()->addText("DebugDrawer DrawSolidCircle not implemented\n");
#else
	DrawCircle(center, radius, color);
#endif
}

/// Draw a line segment.
void Box2DDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	if(!_enabled)
		return;

	_lineVertices->push_back(b2Vec2ToOsgVec3(p1, drawZCoordinate));
	_lineVertices->push_back(b2Vec2ToOsgVec3(p2, drawZCoordinate));

	osg::Vec4 osgColor = b2ColorToOsgVec4(color);
	_lineColors->push_back(osgColor);
	_lineColors->push_back(osgColor);
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void Box2DDebugDrawer::DrawTransform(const b2Transform& xf)
{
	if(!_enabled)
		return;

	//TODO:
	getDebugDisplayer()->addText("DebugDrawer DrawTransform not implemented\n");
}
