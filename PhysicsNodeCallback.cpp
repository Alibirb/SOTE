#include "PhysicsNodeCallback.h"

PhysicsNodeCallback::PhysicsNodeCallback(osg::PositionAttitudeTransform *transformNode, b2Body *physicsBody, osg::Vec3 box2DToOsgAdjustment)
{
	this->transformNode = transformNode;
	this->physicsBody = physicsBody;
	this->box2DToOsgAdjustment = box2DToOsgAdjustment;
}

void PhysicsNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	transformNode->setPosition(osg::Vec3(physicsBody->GetPosition().x, physicsBody->GetPosition().y, transformNode->getPosition().z() ) + box2DToOsgAdjustment);

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}
