#include "ObjectOverlay.h"


#include <osgwTools/Shapes.h>

#include <osgbCollision/ComputeTriMeshVisitor.h>

#include <osg/Point>

#include "GameObject.h"


ObjectOverlay::ObjectOverlay()
{
	_geode = new osg::Geode();
	//_geometry = new osg::Geometry();
	//_geode->addDrawable(_geometry);

	_geometry = new osg::Geometry;
	//_geometry->setDataVariance(osg::Object::DYNAMIC);
	_geometry->setUseDisplayList(false);
	_geometry->setUseVertexBufferObjects(false);
	_geode->addDrawable(_geometry);

	 {
        osg::StateSet* ss = _geode->getOrCreateStateSet();
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        ss->setMode( GL_POINT_SMOOTH, osg::StateAttribute::ON );
        osg::Point* point = new osg::Point( 20. );
        ss->setAttributeAndModes( point, osg::StateAttribute::ON );
    }

	//_triVerts = new osg::Vec3Array();
	//_geometry->setVertexArray( _triVerts );
	//_triColors = new osg::Vec4Array();
	//_geometry->setColorArray( _triColors );
	//_geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );


	_switchNode = new osg::Switch();
	_switchNode->addChild(_geode);


	setActive(false);
}
ObjectOverlay::ObjectOverlay(GameObject* object) : ObjectOverlay()
{
	_object = object;
}

ObjectOverlay::~ObjectOverlay()
{
	//dtor
}


osg::Node* ObjectOverlay::getNode()
{
	return _switchNode;
}

void ObjectOverlay::setActive(bool active)
{
	if(active)
		_switchNode->setAllChildrenOn();
	else
		_switchNode->setAllChildrenOff();
}


void ObjectOverlay::computeGeometry()
{
	/// NOTE: The way this works is pretty bad; it uses an array of triangles instead of just an array of lines. The result is that any line that's part of more than one triangle is drawn multiple times. But it works.

	if(_geometry->getNumPrimitiveSets())
		_geometry->removePrimitiveSet(0);

	osgbCollision::ComputeTriMeshVisitor visitor;
	_object->getModelNode()->accept(visitor);

	osg::Vec3Array* vertices = visitor.getTriMesh();

	_geometry->setVertexArray(vertices);

	/// Set up the color array.
	osg::Vec4 color = osg::Vec4(1, .5, 0, 1);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
	for(int i = 0; i < vertices->size(); ++i)
		colors->push_back(color);

	_geometry->setColorArray(colors);
	_geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//_geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, vertices->size()));
	_geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices->size()));
}

