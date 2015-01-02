#ifndef OBJECTOVERLAY_H
#define OBJECTOVERLAY_H


#include <osg/Geode>
#include <osg/Switch>


class GameObject;


/// Overlay for GameObjects in the editor. Mainly used to highlight the currently-selected objects
class ObjectOverlay
{
protected:
	osg::ref_ptr<osg::Geode> _geode;
	osg::ref_ptr<osg::Switch> _switchNode;
	osg::ref_ptr<osg::Geometry> _geometry;

	GameObject* _object;


public:
	ObjectOverlay();
	ObjectOverlay(GameObject* object);
	virtual ~ObjectOverlay();

	osg::Node* getNode();

	void setActive(bool active);

	void computeGeometry();	/// Computes the proper geometry to overlay the object.

protected:


};

#endif // OBJECTOVERLAY_H
