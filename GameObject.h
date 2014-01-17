#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>

#ifdef USE_BOX2D_PHYSICS
	#include "Box2DIntegration.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
#endif


/// Class for an object
class GameObject
{
protected:
	bool _useSpriteAsModel = false;
	osg::Vec3 initialPosition;

	osg::ref_ptr<osg::Node> _modelNode;
	osg::ref_ptr<osg::PositionAttitudeTransform> _transformNode;
#ifdef USE_BOX2D_PHYSICS
	b2Body* physicsBody;
#else
	btCollisionObject* _physicsBody;
#endif
	osg::Vec3 physicsToModelAdjustment;	/// adjustment between the visual and physical components

public:
	GameObject();
	virtual ~GameObject();

	virtual void setPosition(osg::Vec3 newPosition);
	virtual osg::Vec3 getLocalPosition();
	virtual osg::Vec3 getWorldPosition();
	osg::Vec3 localToWorld(osg::Vec3 localVector);
	osg::Vec3 worldToLocal(osg::Vec3 worldVector);

	virtual void load(std::string xmlFilename);
	virtual void reset();	/// Reset the object.

	void loadModel(std::string modelFilename);
	void setModelNode(osg::Node* node);

	void parentTo(osg::Group* parent);

	virtual void onUpdate(float deltaTime)=0;
	virtual void onCollision(GameObject* other)=0;

protected:
};

#endif // GAMEOBJECT_H
