#include "Controller.h"

#include "ControlledObject.h"

#include "tinyxml/tinyxml2.h"

#ifndef USE_BOX2D_PHYSICS
	#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#endif // USE_BOX2D_PHYSICS
#include "PhysicsData.h"
#include "PhysicsNodeCallback.h"

#include "AngelScriptEngine.h"

#include "Level.h"

Controller::Controller()
{
	registerController();


	osg::Vec3 position;

	physicsToModelAdjustment = osg::Vec3(0, 0, 0);
	btSphereShape* shape = new btSphereShape(_radius/2);

	btTransform transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(osgbCollision::asBtVector3(position + physicsToModelAdjustment));

	_physicsBody = new btPairCachingGhostObject();
	_physicsBody->setWorldTransform(transform);
	_physicsBody->setCollisionShape(shape);
	getCurrentLevel()->getBulletWorld()->addCollisionObject(_physicsBody, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter);

	_transformNode->setUpdateCallback(new BulletPhysicsNodeCallback(_physicsBody, -physicsToModelAdjustment));

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Controller";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_physicsBody->setUserPointer(userData);
#endif
}
Controller::Controller(XMLElement* xmlElement) : Controller()
{
	load(xmlElement);
}

Controller::~Controller()
{
}

void Controller::load(XMLElement* xmlElement)
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
		else if(elementType == "controlledObject")
			addControlledObject(new ControlledObject(currentElement));
		else if(elementType == "function")
			setFunction(currentElement->Attribute("name"), currentElement->GetText());
	}


	if(xmlElement->Attribute("animation"))
	{
		std::string animation = xmlElement->Attribute("animation");
		if(animation == "true")
			findAnimation();
	}
}


void Controller::addControlledObject(ControlledObject* object)
{
	_controlled.push_back(object);
}

void Controller::onPlayerInteraction()
{
	std::cout << "Player interacted." << std::endl;
	if(_functions["onInteraction"])
		getScriptEngine()->runFunction(_functions["onInteraction"], "object", this);
}

void Controller::sendMessage(std::string& message)
{
	std::cout << message << std::endl;
	for(ControlledObject* object : _controlled)
		object->receiveMessage(message);
}

void Controller::setFunction(std::string functionName, std::string code)
{
	_functions[functionName] = getScriptEngine()->compileFunction("Controller", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}



namespace AngelScriptWrapperFunctions
{
	Controller* ControllerFactoryFunction()
	{
		return new Controller();
	}
	void ControllerDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((Controller*)memory)->~Controller();
	}

}

using namespace AngelScriptWrapperFunctions;


void registerController()
{
	static bool registered = false;
	if(registered)
		return;

	registerGameObject();

	getScriptEngine()->registerObjectType("Controller", sizeof(Controller), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("Controller", "Controller@ f()", asFUNCTION(ControllerFactoryFunction));

	getScriptEngine()->registerObjectMethod("Controller", "void sendMessage(const string &in)", asMETHOD(Controller, sendMessage), asCALL_THISCALL);

	registered = true;
}

