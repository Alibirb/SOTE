#include "PhysicsNodeCallback.h"

void PhysicsNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	transformNode->setPosition(osg::Vec3(physicsBody->GetPosition().x, physicsBody->GetPosition().y, transformNode->getPosition().z() ) + box2DToOsgAdjustment);

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}
