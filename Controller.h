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

public:
	Controller();
	Controller(GameObjectData* dataObj);
	virtual ~Controller();

	void addControlledObject(ControlledObject* object);

	void onPlayerInteraction();	/// Called when the player interacts with the object.

	void sendMessage(std::string& message);


	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveControllerVariables(GameObjectData* dataObj);	/// Saves the variables declared in Controller
	void loadControllerVariables(GameObjectData* dataObj);	/// Loads the variables declared in Controller
};

void registerController();

#endif // CONTROLLER_H
