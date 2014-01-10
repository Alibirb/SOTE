#ifndef PHYSICSNODECALLBACK_H
#define PHYSICSNODECALLBACK_H


#include "globals.h"
#include <osg/PositionAttitudeTransform>

#ifdef USE_BOX2D_PHYSICS
	#include "Box2D/Box2D.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
#endif


#ifdef USE_BOX2D_PHYSICS
class Box2DPhysicsNodeCallback : public osg::NodeCallback
{
private:
	b2Body *physicsBody;
	osg::Vec3 physicsToOsgAdjustment;	//adjustment between the visual and physical components
public:
	Box2DPhysicsNodeCallback::Box2DPhysicsNodeCallback(b2Body *physicsBody, osg::Vec3 box2DToOsgAdjustment);

	Box2DPhysicsNodeCallback(b2Body *physicsBody, osg::Vec3 box2DToOsgAdjustment);

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#else

// from bullet-test (slightly modified)
class BulletPhysicsNodeCallback: public osg::NodeCallback
{
private:
	btCollisionObject* physicsBody;
	osg::Vec3 physicsToOsgAdjustment;	// Adjustment between the visual and physical components

public:
	BulletPhysicsNodeCallback(btCollisionObject *physicsBody, osg::Vec3 physicsToOsgAdjustment)
	{
		this->physicsBody = physicsBody;
		this->physicsToOsgAdjustment = physicsToOsgAdjustment;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::Matrix mat = osgbCollision::asOsgMatrix(physicsBody->getWorldTransform());

		osg::PositionAttitudeTransform *pat = dynamic_cast<osg::PositionAttitudeTransform *> (node);
		pat->setPosition(mat.getTrans() + physicsToOsgAdjustment);
		pat->setAttitude(mat.getRotate());

		traverse(node, nv);
	}
};

#endif	// USE_BOX2D_PHYSICS

#endif // PHYSICSNODECALLBACK_H
