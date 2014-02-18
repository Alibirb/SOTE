#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>

#include "ImprovedAnimationManager.h"

#ifdef USE_BOX2D_PHYSICS
	#include "Box2DIntegration.h"
#else
	#include "btBulletDynamicsCommon.h"
	#include "osgbCollision/Utils.h"
#endif



class GameObjectData;

namespace tinyxml2
{
	class XMLElement;
}
using namespace tinyxml2;

/// Class for an object
class GameObject
{
protected:
	bool _useSpriteAsModel = false;
	osg::Vec3 initialPosition;

	ImprovedAnimationManager* _animationManager;
	osg::ref_ptr<osg::PositionAttitudeTransform> _transformNode;
	osg::ref_ptr<osg::Node> _modelNode;
	osg::ref_ptr<osg::Node> _updateNode;	// Used to update the object each frame.

#ifdef USE_BOX2D_PHYSICS
	b2Body* physicsBody;
#else
	btCollisionObject* _physicsBody;
#endif
	osg::Vec3 physicsToModelAdjustment;	/// adjustment between the visual and physical components

	// Export/meta data (used for editor purposes)
	std::string _modelFilename;
public:
	std::string _objectType = "GameObject";

public:
	GameObject();
	GameObject(XMLElement* xmlElement);
	GameObject(GameObjectData* dataObj);
	virtual ~GameObject();

	virtual void setPosition(osg::Vec3 newPosition);
	virtual osg::Vec3 getLocalPosition();
	virtual osg::Vec3 getWorldPosition();
	osg::Vec3 localToWorld(osg::Vec3 localVector);
	osg::Vec3 worldToLocal(osg::Vec3 worldVector);

	void loadFromFile(std::string xmlFilename, std::string searchPath="media/Objects/");
	virtual void load(XMLElement* xmlElement);
	virtual void reset();	/// Reset the object.


	virtual GameObjectData* save();	/// Saves the object's data (should call a function to save the variables for each class the object overrides)
	virtual void load(GameObjectData* dataObj);

	void loadModel(std::string modelFilename);
	void setModelNode(osg::Node* node);

	void parentTo(osg::Group* parent);
	void unparentFrom(osg::Group* parent);

	virtual void onUpdate(float deltaTime){};
	virtual void onCollision(GameObject* other){};	/// Called when two objects collide. Ideally should check the type of the other object, and call a more appropriate onCollision method.

	bool findAnimation();

	void playAnimation(std::string& animationName);

protected:
	void saveGameObjectVariables(GameObjectData* dataObj);	/// Saves the variables declared in GameObject.
	void loadGameObjectVariables(GameObjectData* dataObj);
};

void registerGameObject();

#endif // GAMEOBJECT_H
