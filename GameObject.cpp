#include "GameObject.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "globals.h"
#include "Level.h"
#include "Sprite.h"
#include "OwnerUpdateCallback.h"

GameObject::GameObject()
{
	_transformNode = new osg::PositionAttitudeTransform();
	addToSceneGraph(_transformNode);
}

GameObject::~GameObject()
{
	if(_transformNode->getNumParents() == 0)
		logError("GameObject with no parents found.");
	if(_transformNode->getNumParents() > 1)
		logError("GameObject with multiple parents found.");
	_transformNode->getParent(0)->removeChild(_transformNode);	// remove the node from the scenegraph.

	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		getCurrentLevel()->getPhysicsWorld()->DestroyBody(physicsBody);
#else
		getCurrentLevel()->getBulletWorld()->removeCollisionObject(_physicsBody);
		delete _physicsBody;
#endif
	}
}

void GameObject::loadModel(std::string modelFilename)
{
	if(modelFilename.length() > 4 && modelFilename.substr(modelFilename.length() - 4, 4) == ".png")
		_useSpriteAsModel = true;
	if(_useSpriteAsModel)
	{
		if(!dynamic_pointer_cast<Sprite>(_modelNode))
			setModelNode(new Sprite(modelFilename));
		else
			dynamic_pointer_cast<Sprite>(_modelNode)->setImage(modelFilename);
	}

	else
	{
		_transformNode->removeChild(_modelNode);
		_modelNode = osgDB::readNodeFile(modelFilename);
		if(!_modelNode)
			logError("Could not load node file \"" + modelFilename + "\"");
		_modelNode->setUpdateCallback(new OwnerUpdateCallback<GameObject>(this));	/// NOTE: Due to virtual inheritance, should be able to remove the template-ness of OwnerUpdateCallback.
		_transformNode->addChild(_modelNode);
	}
}
void GameObject::setModelNode(osg::Node* node)
{
	_transformNode->removeChild(_modelNode);
	_modelNode = node;
	_modelNode->setUpdateCallback(new OwnerUpdateCallback<GameObject>(this));	/// NOTE: Due to virtual inheritance, should be able to remove the template-ness of OwnerUpdateCallback.
	_transformNode->addChild(_modelNode);
}


void GameObject::setPosition(osg::Vec3 newPosition)
{
	this->_transformNode->setPosition(newPosition);
	if(_physicsBody)
	{
#ifdef USE_BOX2D_PHYSICS
		// TODO
#else
		btTransform transform;
		transform = _physicsBody->getWorldTransform();
		//transform.setOrigin(osgbCollision::asBtVector3(localToWorld(newPosition - physicsToModelAdjustment)));
		transform.setOrigin(osgbCollision::asBtVector3(newPosition - physicsToModelAdjustment));	/// FIXME: should convert to world coordinates.
		_physicsBody->setWorldTransform(transform);
#endif // USE_BOX2D_PHYSICS
	}
}

osg::Vec3 GameObject::getLocalPosition()
{
	return _transformNode->getPosition();
}
osg::Vec3 GameObject::getWorldPosition()
{
	return getWorldCoordinates(_transformNode)->getTrans();
}
osg::Vec3 GameObject::localToWorld(osg::Vec3 localVector)	/// FIXME: does not seem to work.
{
	osg::NodePathList paths = _transformNode->getParentalNodePaths();
	osg::Matrix localToWorldMatrix = osg::computeLocalToWorld(paths.at(0));
	osg::Vec3f worldVector = localToWorldMatrix * localVector;
	return worldVector;
}
osg::Vec3 GameObject::worldToLocal(osg::Vec3 worldVector)	/// FIXME: May not work, either.
{
	osg::NodePathList paths = _transformNode->getParentalNodePaths();
	osg::Matrix worldToLocalMatrix = osg::computeWorldToLocal(paths.at(0));
	osg::Vec3f localVector = worldToLocalMatrix * worldVector;
	return localVector;
}

void GameObject::load(std::string xmlFilename)
{

}
void GameObject::reset()
{
	this->setPosition(initialPosition);
}

void GameObject::parentTo(osg::Group* parent)
{
	if(_transformNode->getNumParents() > 0)
	{
		logWarning("GameObject already parented.");
		_transformNode->getParent(0)->removeChild(_transformNode);	// Remove from current parent.
	}
	parent->addChild(_transformNode);
}