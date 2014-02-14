#include "StateMachine.h"

#include "globals.h"

#include "tinyxml/tinyxml2.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"


State::State(GameObject* owner, XMLElement* xmlElement) : _onEnterFunction(NULL), _onUpdateFunction(NULL), _onExitFunction(NULL)
{
	_owner = owner;
	load(xmlElement);
}
State::~State(){};

void State::onEnter()
{
	if(_onEnterFunction)
		getScriptEngine()->runFunction(_onEnterFunction, "object", _owner);
};
void State::onUpdate(float deltaTime)
{
	if(_onUpdateFunction)
		getScriptEngine()->runFunction(_onUpdateFunction, "object", _owner);
};
void State::onExit()
{
	if(_onExitFunction)
		getScriptEngine()->runFunction(_onExitFunction, "object", _owner);
};

void State::load(XMLElement* xmlElement)
{
	//if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
	//	load(xmlElement->Attribute("source"));

	if(xmlElement->Attribute("name"))
		_name = xmlElement->Attribute("name");

	XMLElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "onEnter")
			setOnEnterScriptFunction(currentElement->GetText());
		else if(elementType == "onUpdate")
			setOnUpdateScriptFunction(currentElement->GetText());
		else if(elementType == "onExit")
			setOnExitScriptFunction(currentElement->GetText());
	}

}

GameObjectData* State::save()
{
	GameObjectData* dataObj = new GameObjectData("state");

	saveStateVariables(dataObj);

	return dataObj;
}
void State::saveStateVariables(GameObjectData* dataObj)
{
	dataObj->addData("name", _name);

	if(_onEnterFunction)
		dataObj->addScriptFunction("onEnter", _onEnterCode);
	if(_onUpdateFunction)
		dataObj->addScriptFunction("onUpdate", _onUpdateCode);
	if(_onExitFunction)
		dataObj->addScriptFunction("onExit", _onExitCode);
}

void State::setOnEnterScriptFunction(std::string code)
{
	_onEnterCode = code;
	_onEnterFunction = getScriptEngine()->compileFunction("State", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}
void State::setOnUpdateScriptFunction(std::string code)
{
	_onUpdateCode = code;
	_onUpdateFunction = getScriptEngine()->compileFunction("State", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}
void State::setOnExitScriptFunction(std::string code)
{
	_onExitCode = code;
	_onExitFunction = getScriptEngine()->compileFunction("State", code.c_str(), 0, asCOMP_ADD_TO_MODULE);
}

std::string& State::getName()
{
	return _name;
}



StateMachine::StateMachine()
{
	//ctor
}

StateMachine::~StateMachine()
{
	//dtor
}

void StateMachine::setCurrentState(std::string stateName)
{
	if(!_states[stateName])
	{
		logError("Could not find state '" + stateName + "'.");
		return;
	}
	_currentState = _states[stateName];
}

void StateMachine::changeState(std::string& stateName)
{
	if(!_states[stateName])
	{
		logError("Could not find state '" + stateName + "'.");
		return;
	}

	changeState(_states[stateName]);
}
void StateMachine::changeState(State* newState)
{
	_previousState = _currentState;
	_currentState->onExit();
	_currentState = newState;
	_currentState->onEnter();
}

void StateMachine::revertToPreviousState()
{
	changeState(_previousState);
}

std::string& StateMachine::getStateName()
{
	return _currentState->getName();
}

void StateMachine::onUpdate(float deltaTime)
{
	if(_globalState)
		_globalState->onUpdate(deltaTime);
	if(_currentState)
		_currentState->onUpdate(deltaTime);
}

GameObjectData* StateMachine::save()
{
	GameObjectData* dataObj = new GameObjectData("stateMachine");

	saveStateMachineVariables(dataObj);

	return dataObj;
}

void StateMachine::saveStateMachineVariables(GameObjectData* dataObj)
{
	for(auto kv : _states)
		dataObj->addChild(kv.second->save());
}
