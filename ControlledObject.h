#ifndef CONTROLLEDOBJECT_H
#define CONTROLLEDOBJECT_H

#include "GameObject.h"
#include "StateMachine.h"


class asIScriptFunction;

class Controller;

class GameObjectData;

class ControlledObject : public GameObject
{
protected:
	StateMachine* _stateMachine;

public:
	ControlledObject();
	ControlledObject(GameObjectData* dataObj);
	virtual ~ControlledObject();

	void changeState(std::string& stateName);
	std::string getCurrentStateName();

	void receiveMessage(std::string& message);

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveControlledObjectVariables(GameObjectData* dataObj);	/// Saves variables declared in ControlledObject
	void loadControlledObjectVariables(GameObjectData* dataObj);	/// Loads variables declared in ControlledObject


};


void registerControlledObject();

#endif // CONTROLLEDOBJECT_H
