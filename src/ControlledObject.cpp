#include "ControlledObject.h"


#include "globals.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"


ControlledObject::ControlledObject(osg::Group* parentNode) : GameObject(parentNode)
{
	_objectType = "ControlledObject";
	registerControlledObject();
	_stateMachine = new StateMachine(this);
}
ControlledObject::ControlledObject(GameObjectData* dataObj, osg::Group* parentNode) : ControlledObject(parentNode)
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
	GameObjectData* dataObj = new GameObjectData();

	saveSaveableVariables(dataObj);
	saveGameObjectVariables(dataObj);
	saveControlledObjectVariables(dataObj);

	return dataObj;
}
void ControlledObject::saveControlledObjectVariables(GameObjectData* dataObj)
{
	//dataObj->addChild(_stateMachine->save());
	if(_stateMachine)
		dataObj->addData("stateMachine", _stateMachine->save());
}
void ControlledObject::load(GameObjectData* dataObj)
{
	loadSaveableVariables(dataObj);
	loadGameObjectVariables(dataObj);
	loadControlledObjectVariables(dataObj);
}

void ControlledObject::loadControlledObjectVariables(GameObjectData* dataObj)
{
	if(dataObj->getObject("stateMachine"))
		_stateMachine->load(dataObj->getObject("stateMachine"));
	else
		_stateMachine->addState(new State(this), "default");
}







namespace AngelScriptWrapperFunctions
{
	ControlledObject* ControlledObjectFactoryFunction()
	{
		return new ControlledObject(root);
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
