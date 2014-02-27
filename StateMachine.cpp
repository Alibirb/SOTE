#include "StateMachine.h"

#include "globals.h"

#include "AngelScriptEngine.h"

#include "GameObjectData.h"


State::State(GameObject* owner, GameObjectData* dataObj) : _onEnterFunction(NULL), _onUpdateFunction(NULL), _onExitFunction(NULL)
{
	_owner = owner;
	load(dataObj);
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

GameObjectData* State::save()
{
	GameObjectData* dataObj = new GameObjectData("State");

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

void State::load(GameObjectData* dataObj)
{
	loadStateVariables(dataObj);
}
void State::loadStateVariables(GameObjectData* dataObj)
{
	_name = dataObj->getString("name");

	if(dataObj->hasFunctionSource("onEnter"))
		setOnEnterScriptFunction(dataObj->getFunctionSource("onEnter"));
	if(dataObj->hasFunctionSource("onUpdate"))
		setOnUpdateScriptFunction(dataObj->getFunctionSource("onUpdate"));
	if(dataObj->hasFunctionSource("onExit"))
		setOnExitScriptFunction(dataObj->getFunctionSource("onExit"));
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



StateMachine::StateMachine(GameObject* owner)
{
	_owner = owner;
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
	GameObjectData* dataObj = new GameObjectData("StateMachine");

	saveStateMachineVariables(dataObj);
	return dataObj;
}

void StateMachine::saveStateMachineVariables(GameObjectData* dataObj)
{
	for(auto kv : _states)
		dataObj->addChild(kv.second->save());

}

void StateMachine::load(GameObjectData* dataObj)
{
	loadStateMachineVariables(dataObj);
}

void StateMachine::loadStateMachineVariables(GameObjectData* dataObj)
{
	for(auto child : dataObj->getChildren())
	{
		if(child->getType() == "State")
			addState(new State(_owner, child), child->getString("name"));
		else
			logWarning("No frickin' clue what this non-State object is doing as a child of a StateMachine");
	}
	_defaultState = dataObj->getString("defaultState");
	setCurrentState(_defaultState);
}
