#ifndef PLAYER_H
#define PLAYER_H

#include "Fighter.h"
#include <unordered_map>

class Weapon;


#define DEFAULT_PLAYER_IMAGE "player.png"


class Enemy;

class Player : public Fighter
{
protected:

public:
	Player(std::string name, osg::Vec3 position);

	/// Handles moving the player according to the given input vector.
	virtual void processMovementControls(osg::Vec3 controlVector);

	/// Handles keyboard input that doesn't map precisely to a single action (e.g. if the action is context-sensitive or varies by character.) Should not be used for standard input (eg WASD movement keys, jump button, etc.). If there's a standard Player function that could be called instead of this, call it.
	virtual void processButton(int key){};

	bool isActivePlayer();

	virtual void setPosition(osg::Vec3 newPosition);

	/// Returns the coordinates that the camera should focus on.
	virtual osg::Vec3 getCameraTarget();

	void attack(Enemy *theOneWhoMustDie);

	void die();

	void onUpdate(float deltaTime);

	virtual ~Player();
protected:
private:
};



Player* getClosestPlayer(osg::Vec3 position);
Player* getActivePlayer();
void setActivePlayer(std::string newActivePlayer);
void addNewPlayer(std::string playerName, osg::Vec3 position = osg::Vec3(0,0,0));
std::unordered_map<std::string, Player*> getPlayers();

#endif // PLAYER_H
