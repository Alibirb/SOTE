#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <unordered_map>

class asIScriptFunction;
class GameObject;

namespace tinyxml2
{
	class XMLElement;
}

using namespace tinyxml2;

class GameObjectData;


class State
{
protected:
	GameObject* _owner;
	std::string _name;
	asIScriptFunction* _onEnterFunction;
	asIScriptFunction* _onUpdateFunction;
	asIScriptFunction* _onExitFunction;

	// editor data
	std::string _onEnterCode;
	std::string _onUpdateCode;
	std::string _onExitCode;

public:
	State(GameObject* owner, XMLElement* xmlElement);
	State(GameObject* owner, GameObjectData* dataObj);
	virtual ~State();

	virtual void onEnter();
	virtual void onUpdate(float deltaTime);
	virtual void onExit();

	void setOnEnterScriptFunction(std::string code);
	void setOnUpdateScriptFunction(std::string code);
	void setOnExitScriptFunction(std::string code);

	void load(XMLElement* xmlElement);

	GameObjectData* save();
	void load(GameObjectData* dataObj);

	std::string& getName();

protected:
	void saveStateVariables(GameObjectData* dataObj);	/// Saves the variables declared in State
	void loadStateVariables(GameObjectData* dataObj);	/// Loads the variables declared in State
};

class StateMachine
{
private:
	State* _currentState;
	State* _previousState;
	State* _globalState;
	std::unordered_map<std::string, State*> _states;

	GameObject* _owner;

public:
	StateMachine(GameObject* owner);
	virtual ~StateMachine();
	void setGlobalState(State* newGlobalState)
	{
		_globalState = newGlobalState;
	}
	void setCurrentState(std::string stateName);
	void addState(State* newState, std::string stateName)
	{
		_states[stateName] = newState;
	}
	void changeState(std::string& stateName);
	void changeState(State* newState);
	void revertToPreviousState();

	std::string& getStateName();

	void onUpdate(float deltaTime);

	GameObjectData* save();
	void load(GameObjectData* dataObj);

protected:
	void saveStateMachineVariables(GameObjectData* dataObj);
	void loadStateMachineVariables(GameObjectData* dataObj);
};

#endif // STATEMACHINE_H
