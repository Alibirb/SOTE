#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "GameObject.h"
#include <vector>
#include <unordered_map>

class asIScriptFunction;

class ControlledObject;

/// Class for an object (e.g. light switch) that controls other objects (e.g. lights).
/// TODO: should inherit a class called Interactable
class Controller : public GameObject
{
protected:
	std::vector<ControlledObject*> _controlled;
	float _radius = 5.0;	/// how close the player needs to be to interact with the object.
	std::unordered_map<std::string, asIScriptFunction*> _functions;	/// script functions

public:
	Controller();
	Controller(TiXmlElement* xmlElement);
	virtual ~Controller();
	void load(TiXmlElement* xmlElement);

	void addControlledObject(ControlledObject* object);

	void onPlayerInteraction();	/// Called when the player interacts with the object.

	void sendMessage(std::string& message);

	void setFunction(std::string functionName, std::string code);
};

void registerController();

#endif // CONTROLLER_H
