#ifndef CONTROLLEDOBJECT_H
#define CONTROLLEDOBJECT_H

#include "GameObject.h"
#include "StateMachine.h"


class asIScriptFunction;

class Controller;

class ControlledObject : public GameObject
{
protected:
	StateMachine* _stateMachine;
	std::unordered_map<std::string, asIScriptFunction*> _onMessage;	/// the functions to call for each message received.

public:
	ControlledObject();
	ControlledObject(XMLElement* xmlElement);
	virtual ~ControlledObject();
	void load(XMLElement* xmlElement);

	void changeState(std::string& stateName);
	std::string getCurrentStateName();

	void receiveMessage(std::string& message);

	void setOnMessageFunction(std::string message, std::string code);

};


void registerControlledObject();

#endif // CONTROLLEDOBJECT_H
