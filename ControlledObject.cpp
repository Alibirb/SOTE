#include "ControlledObject.h"


#include "globals.h"

#include "tinyxml/tinyxml2.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"


ControlledObject::ControlledObject()
{
	_objectType = "ControlledObject";
	registerControlledObject();
	_stateMachine = new StateMachine(this);
}
ControlledObject::ControlledObject(GameObjectData* dataObj) : ControlledObject()
{
	load(dataObj);
}

ControlledObject::~ControlledObject()
{
	//dtor
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
	if(_functions["onMessage_"+message])
		getScriptEngine()->runFunction(_functions["onMessage_"+message], "object", this);
}



GameObjectData* ControlledObject::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);
	saveControlledObjectVariables(dataObj);

	return dataObj;
}
void ControlledObject::saveControlledObjectVariables(GameObjectData* dataObj)
{
	dataObj->addChild(_stateMachine->save());
}
void ControlledObject::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadControlledObjectVariables(dataObj);
}

void ControlledObject::loadControlledObjectVariables(GameObjectData* dataObj)
{
	//dataObj->addChild(_stateMachine->save());
	for(auto child :dataObj->getChildren())
	{
		if(child->getType() == "StateMachine")
			_stateMachine->load(child);
		else
			logWarning("No frickin' clue what this non-stateMachine object is doing as a child of a ControlledObject");
	}
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
