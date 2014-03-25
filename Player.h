#ifndef PLAYER_H
#define PLAYER_H

#include "Fighter.h"
#include <unordered_map>


#define DEFAULT_PLAYER_IMAGE "player.png"

class Controller;

class Enemy;

class Player : public Fighter
{
protected:
	Controller* _willInteractWith;	/// nearest interactable object (or none if none are in range)


public:
	Player(std::string name, osg::Vec3 position);
	Player(GameObjectData* dataObj);

	/// Handles moving the player according to the given input vector.
	void processMovementControls(osg::Vec3 controlVector);

	/// Handles keyboard input that doesn't map precisely to a single action (e.g. if the action is context-sensitive or varies by character.) Should not be used for standard input (eg WASD movement keys, jump button, etc.). If there's a standard Player function that could be called instead of this, call it.
	void processButton(int key){};

	bool isActivePlayer();

	/// Returns the coordinates that the camera should focus on.
	osg::Vec3 getCameraTarget();

	//void attack(Fighter *theOneWhoMustDie);

	void die();

	void onUpdate(float deltaTime);
	void onCollision(Controller* controller);
	void onCollision(GameObject* other);

	void interact();	/// Interact with an object if in range.

	virtual ~Player();
protected:
};



Player* getClosestPlayer(osg::Vec3 position);
Player* getActivePlayer();
void setActivePlayer(std::string newActivePlayer);
void addNewPlayer(std::string playerName, osg::Vec3 position = osg::Vec3(0,0,0));
void addPlayer(std::string playerName, Player* thePlayer);
std::unordered_map<std::string, Player*> getPlayers();

#endif // PLAYER_H
