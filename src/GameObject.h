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

#include <unordered_map>


#include "GameObjectData.h"

#include "ImprovedMatrixTransform.h"


class asIScriptFunction;

class Attachment;

class ObjectOverlay;


/// Class for an object
class GameObject : public Saveable
{
protected:
	osg::Vec3 initialPosition;

	ImprovedAnimationManager* _animationManager;
	osg::ref_ptr<ImprovedMatrixTransform> _transformNode;
	osg::ref_ptr<osg::Node> _modelNode;
	osg::ref_ptr<osg::Node> _updateNode;	/// Used to update the object each frame.

#ifdef USE_BOX2D_PHYSICS
	b2Body* physicsBody;
#else
	btCollisionObject* _physicsBody;
#endif
	osg::Vec3 physicsToModelAdjustment;	/// adjustment between the visual and physical components

	std::unordered_map<std::string, asIScriptFunction*> _functions;	/// script functions


	/// Export/meta data (used for editor purposes)
	std::string _modelFilename;
	std::unordered_map<std::string, std::string> _functionSources;	/// source code for script functions.
	std::string _collisionShapeGenerationMethod;

	ObjectOverlay* _overlay;

	osg::Program* _shaderProgram;

	std::list<Attachment*> _attachments;


protected:

	GameObject(osg::Group* parentNode);
	GameObject(GameObjectData* dataObj, osg::Group* parentNode);

public:
	virtual ~GameObject();

	static GameObject* create(GameObjectData* dataObj, osg::Group* parentNode);

	virtual void setPosition(osg::Vec3 newPosition);
	virtual void setRotation(osg::Quat newRotation);
	virtual void setScale(osg::Vec3 newScale);
	virtual osg::Vec3 getLocalPosition();
	virtual osg::Vec3 getWorldPosition();
	virtual osg::Quat getLocalRotation();
	virtual osg::Quat getWorldRotation();
	virtual osg::Vec3 getScale();
	virtual void translate(osg::Vec3 translation);
	virtual void rotate(osg::Quat rotation);
	osg::Vec3 localToWorld(osg::Vec3 localVector);
	osg::Vec3 worldToLocal(osg::Vec3 worldVector);
	virtual void recalculatePhysicsTransform();

	osg::ref_ptr<ImprovedMatrixTransform> getTransformNode()
	{
		return _transformNode;
	}

	virtual void reset();	/// Reset the object.


	virtual GameObjectData* save();	/// Saves the object's data (should call a function to save the variables for each class the object overrides)
	virtual void load(GameObjectData* dataObj);

	void loadModel(std::string modelFilename);
	void setModelNode(osg::Node* node);
	osg::Node* getModelNode();

	void parentTo(osg::Group* parent);
	void unparentFrom(osg::Group* parent);

	virtual void onUpdate(float deltaTime){};
	virtual void onCollision(GameObject* other){};	/// Called when two objects collide. Ideally should check the type of the other object, and call a more appropriate onCollision method.

	bool findAnimation();

	void playAnimation(std::string& animationName);

	void setFunction(std::string functionName, std::string code);

	void generateRigidBody(double mass, std::string generationMethod);	/// Generates a rigid body to fit the geometry

#ifndef USING_BOX2D_PHYSICS
	btCollisionObject* getPhysicsBody();
#endif

	void addAttachment(Attachment* attachment);

	ObjectOverlay* getSelectionOverlay();


protected:
	void saveGameObjectVariables(GameObjectData* dataObj);	/// Saves the variables declared in GameObject.
	void loadGameObjectVariables(GameObjectData* dataObj);

	void tryToSetProperShaders(osg::Node* node);	/// Attempts to setup shaders for drawables contained in the given node. (Recursive)

	/**
	 * Sets up the default shaders for multitextured geometry.
	 * Shader Unit 0	diffuse
	 * Shader Unit 1	normalmap
	 * Shader Unit 2	heightmap (optional)
	 */
	void createShaders();
};

void registerGameObject();

#endif // GAMEOBJECT_H
