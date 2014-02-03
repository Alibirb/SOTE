#include "ControlledObject.h"


#include "globals.h"

#include "tinyxml/tinyxml.h"

#include "AngelScriptEngine.h"


ControlledObject::ControlledObject()
{
	registerControlledObject();
	_stateMachine = new StateMachine();
}

ControlledObject::ControlledObject(TiXmlElement* xmlElement) : ControlledObject()
{
	load(xmlElement);
}

ControlledObject::~ControlledObject()
{
	//dtor
}

void ControlledObject::load(TiXmlElement* xmlElement)
{
	if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
		loadFromFile(xmlElement->Attribute("source"));

	float x, y, z;
	xmlElement->QueryFloatAttribute("x", &x);
	xmlElement->QueryFloatAttribute("y", &y);
	xmlElement->QueryFloatAttribute("z", &z);

	initialPosition = osg::Vec3(x, y, z);
	setPosition(initialPosition);



	TiXmlElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			loadModel(currentElement->Attribute("source"));
		else if(elementType == "state")
			_stateMachine->addState(new State(this, currentElement), currentElement->Attribute("name"));
		else if(elementType == "onMessage")
			setOnMessageFunction(currentElement->Attribute("message"), currentElement->GetText());
	}


	if(xmlElement->Attribute("animation"))
	{
		std::string animation = xmlElement->Attribute("animation");
		if(animation == "true")
			findAnimation();
	}
	if(xmlElement->Attribute("channel"))
		_channel = xmlElement->Attribute("channel");

	if(xmlElement->Attribute("defaultState"))
		_stateMachine->setCurrentState(xmlElement->Attribute("defaultState"));

}


void ControlledObject::changeState(std::string& stateName)
{
	_stateMachine->changeState(stateName);
}
std::string ControlledObject::getCurrentStateName()
{
	return _stateMachine->getStateName();
}

void ControlledObject::receiveMessage(std::string& message)
{
	if(_onMessage[message])
		//getScriptEngine()->runMethod(this, _onMessage[message]);
		getScriptEngine()->runFunction(_onMessage[message], "object", this);
}

void ControlledObject::setOnMessageFunction(std::string message, std::string code)
{
	_onMessage[message] = getScriptEngine()->compileFunction("ControlledObject", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}



namespace AngelScriptWrapperFunctions
{
/*	void GameObjectConstructor(GameObject *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) GameObject();
	}*/
	ControlledObject* ControlledObjectFactoryFunction()
	{
		return new ControlledObject();
	}
	/*void GameObjectInitConstructor(Damages damages, std::string imageFilename, GameObject *self)
	{
		new(self) GameObject(damages, imageFilename);
	}
	void GameObjectCopyConstructor(GameObject& other, GameObject* self)
	{
		new(self) GameObject(other.damages, other.imageFilename);
	}*/
	void ControlledObjectDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((ControlledObject*)memory)->~ControlledObject();
	}

}

using namespace AngelScriptWrapperFunctions;


void registerControlledObject()
{
	static bool registered = false;
	if(registered)
		return;

	registerGameObject();

	//getScriptEngine()->registerObjectType("GameObject", 0, asOBJ_REF | asOBJ_NOCOUNT | GetTypeTraits<GameObject>() );
	getScriptEngine()->registerObjectType("ControlledObject", sizeof(ControlledObject), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("ControlledObject", "ControlledObject@ f()", asFUNCTION(ControlledObjectFactoryFunction));
	//getScriptEngine()->registerConstructor("GameObject", "void f(const Damages &in, const string &in)", asFUNCTION(GameObjectInitConstructor));
//	getScriptEngine()->registerConstructor("GameObject", "void f()", asFUNCTION(GameObjectConstructor));
//	getScriptEngine()->registerDestructor("GameObject", asFUNCTION(GameObjectDestructor));
//	getScriptEngine()->registerConstructor("GameObject", "void f(const GameObject &in)", asFUNCTION(GameObjectCopyConstructor));
//	getScriptEngine()->registerObjectProperty("GameObject", "Damages damages", asOFFSET(GameObject, damages));
//	getScriptEngine()->registerObjectProperty("GameObject", "string imageFilename", asOFFSET(GameObject, imageFilename));

	getScriptEngine()->registerObjectMethod("ControlledObject", "void changeState(string &in)", asMETHOD(ControlledObject, changeState), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("ControlledObject", "string getStateName()", asMETHOD(ControlledObject, getCurrentStateName), asCALL_THISCALL);
getScriptEngine()->registerObjectMethod("ControlledObject", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);



	getScriptEngine()->registerObjectMethod("ControlledObject", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	//getScriptEngine()->registerFunction("GameObject loadProjectilePrototype(const string &in)", asFUNCTION(GameObject::loadPrototype));

	registered = true;
}
