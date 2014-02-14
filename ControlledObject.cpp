#include "ControlledObject.h"


#include "globals.h"

#include "tinyxml/tinyxml2.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"


ControlledObject::ControlledObject()
{
	registerControlledObject();
	_stateMachine = new StateMachine();
}

ControlledObject::ControlledObject(XMLElement* xmlElement) : ControlledObject()
{
	load(xmlElement);
}

ControlledObject::~ControlledObject()
{
	//dtor
}

void ControlledObject::load(XMLElement* xmlElement)
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
		getScriptEngine()->runFunction(_onMessage[message], "object", this);
}

void ControlledObject::setOnMessageFunction(std::string message, std::string code)
{
	_onMessage[message] = getScriptEngine()->compileFunction("ControlledObject", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}



GameObjectData* ControlledObject::save()
{
	GameObjectData* dataObj = new GameObjectData("controlledObject");

	saveGameObjectVariables(dataObj);
	saveControlledObjectVariables(dataObj);

	return dataObj;
}

void ControlledObject::saveControlledObjectVariables(GameObjectData* dataObj)
{
	dataObj->addChild(_stateMachine->save());
}


namespace AngelScriptWrapperFunctions
{
	ControlledObject* ControlledObjectFactoryFunction()
	{
		return new ControlledObject();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerControlledObject()
{
	static bool registered = false;
	if(registered)
		return;

	registerGameObject();

	getScriptEngine()->registerObjectType("ControlledObject", sizeof(ControlledObject), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("ControlledObject", "ControlledObject@ f()", asFUNCTION(ControlledObjectFactoryFunction));

	getScriptEngine()->registerObjectMethod("ControlledObject", "void changeState(string &in)", asMETHOD(ControlledObject, changeState), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("ControlledObject", "string getStateName()", asMETHOD(ControlledObject, getCurrentStateName), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("ControlledObject", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("ControlledObject", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	registered = true;
}
