#include "GameObject.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "globals.h"
#include "Level.h"
#include "Sprite.h"
#include "OwnerUpdateCallback.h"
#include "AngelScriptEngine.h"
#include "GameObjectData.h"

#include "tinyxml/tinyxml2.h"


struct AnimationManagerFinder : public osg::NodeVisitor
{
    osg::ref_ptr<ImprovedAnimationManager> _am;
    AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
    void apply(osg::Node& node) {
        if (_am.valid())
            return;
        if (node.getUpdateCallback()) {
            osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
            if (b) {
               _am = new ImprovedAnimationManager(*b);
                return;
            }
        }
        traverse(node);
    }
};




GameObject::GameObject() : _physicsBody(NULL)
{
	registerGameObject();

	_transformNode = new osg::PositionAttitudeTransform();
	addToSceneGraph(_transformNode);
	_updateNode = new osg::Node();
	_updateNode->addUpdateCallback(new OwnerUpdateCallback<GameObject>(this));	/// NOTE: Due to virtual inheritance, should be able to remove the template-ness of OwnerUpdateCallback.
	_transformNode->addChild(_updateNode);
}

GameObject::GameObject(XMLElement* xmlElement) : GameObject()
{
	load(xmlElement);
}
GameObject::GameObject(GameObjectData* dataObj) : GameObject()
{
	load(dataObj);
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
	_modelFilename = modelFilename;
	if(modelFilename.length() > 4 && modelFilename.substr(modelFilename.length() - 4, 4) == ".png")
		_useSpriteAsModel = true;
	if(_useSpriteAsModel)
	{
		if(!osg::dynamic_pointer_cast<Sprite>(_modelNode))
			setModelNode(new Sprite(modelFilename));
		else
			osg::dynamic_pointer_cast<Sprite>(_modelNode)->setImage(modelFilename);
	}
	else
	{
		_transformNode->removeChild(_modelNode);
		_modelNode = osgDB::readNodeFile(modelFilename);
		if(!_modelNode)
			logError("Could not load node file \"" + modelFilename + "\"");
		_transformNode->addChild(_modelNode);
	}
}
void GameObject::setModelNode(osg::Node* node)
{
	_transformNode->removeChild(_modelNode);
	_modelNode = node;
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
		transform.setOrigin(osgbCollision::asBtVector3(newPosition + physicsToModelAdjustment));	/// FIXME: should convert to world coordinates.
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

void GameObject::loadFromFile(std::string xmlFilename, std::string searchPath)
{
	FILE *file = fopen(xmlFilename.c_str(), "rb");
	if(!file)
	{
		xmlFilename = searchPath + xmlFilename;
		file = fopen(xmlFilename.c_str(), "rb");
		if(!file)
			logError("Failed to open file " + xmlFilename);
	}


	XMLDocument doc(xmlFilename.c_str());
	if (doc.LoadFile(file)  != tinyxml2::XML_NO_ERROR)
	{
		logError("Failed to load file " + xmlFilename);
		logError(doc.GetErrorStr1());
	}


	XMLHandle docHandle(&doc);
	XMLElement* rootElement = docHandle.FirstChildElement().ToElement();

	load(rootElement);
}
void GameObject::load(XMLElement* xmlElement)
{
	if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
		loadFromFile(xmlElement->Attribute("source"));

	float x, y, z;
	xmlElement->QueryFloatAttribute("x", &x);
	xmlElement->QueryFloatAttribute("y", &y);
	xmlElement->QueryFloatAttribute("z", &z);

	initialPosition = osg::Vec3(x, y, z);
	setPosition(initialPosition);



	XMLElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			loadModel(currentElement->Attribute("source"));
	}

	if(xmlElement->Attribute("animation"))
	{
		std::string animation = xmlElement->Attribute("animation");
		if(animation == "true")
			findAnimation();
	}

}

GameObjectData* GameObject::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);

	return dataObj;
}
void GameObject::saveGameObjectVariables(GameObjectData* dataObj)
{
	dataObj->addData("position", getLocalPosition());
	dataObj->addData("geometry", _modelFilename);
	// TODO: animation.
}
void GameObject::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
}
void GameObject::loadGameObjectVariables(GameObjectData* dataObj)
{
	setPosition(dataObj->getVec3("position"));
	loadModel(dataObj->getString("geometry"));
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
void GameObject::unparentFrom(osg::Group* parent)
{
	parent->removeChild(_transformNode);
}


bool GameObject::findAnimation()
{
	AnimationManagerFinder finder;
    _modelNode->accept(finder);
    if (finder._am.valid())
	{
        _modelNode->addUpdateCallback(finder._am.get());
        _animationManager = finder._am.get();
        std::cout << "Found animation." << std::endl;
        _animationManager->setPlayMode(osgAnimation::Animation::ONCE);
        return true;
    }
    else
	{
    	std::cout << "Did not find animation." << std::endl;
        //osg::notify(osg::WARN) << "no osgAnimation::AnimationManagerBase found in the subgraph, no animations available" << std::endl;
        return false;
    }
}

void GameObject::playAnimation(std::string& animationName)
{
	_animationManager->play(animationName);
	_animationManager->setPlayMode(osgAnimation::Animation::ONCE);
}




namespace AngelScriptWrapperFunctions
{
	GameObject* GameObjectFactoryFunction()
	{
		return new GameObject();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerGameObject()
{
	static bool registered = false;
	if(registered)
		return;

	getScriptEngine()->registerObjectType("GameObject", sizeof(GameObject), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("GameObject", "GameObject@ f()", asFUNCTION(GameObjectFactoryFunction));

	getScriptEngine()->registerObjectMethod("GameObject", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	registered = true;
}


