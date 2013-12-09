#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Enemy.h"
#include "Weapon.h"


#define MULTIPLE_PLAYERS

#define DEFAULT_PLAYER_IMAGE "player.png"


class Player : public Entity
{
protected:
	Weapon *equipedWeapon;
public:
	Player(std::string name, osg::Vec3 position);

	/// Handles moving the player according to the given input vector.
	virtual void processMovementControls(osg::Vec3 controlVector);

	/// Handles keyboard input that doesn't map precisely to a single action (e.g. if the action is context-sensitive or varies by character.) Should not be used for standard input (eg WASD movement keys, jump button, etc.). If there's a standard Player function that could be called instead of this, call it.
	virtual void processButton(int key){};

	/// Checks for input so the player can respond accordingly.
	virtual void checkForInput();
#ifdef MULTIPLE_PLAYERS
	bool isActivePlayer();
#endif

	virtual void setPosition(osg::Vec3 newPosition);

	/// Returns the coordinates that the camera should focus on.
	virtual osg::Vec3 getCameraTarget() {
		return this->getPosition();
	}

	void attack(Enemy *theOneWhoMustDie) {
		//theOneWhoMustDie->takeDamage(1.0);
		equipedWeapon->fire();
	}

	void equipWeapon(Weapon *theWeapon) {
		equipedWeapon = theWeapon;
	}

	void aimWeapon(Enemy *theOneWhoMustDie) {
		equipedWeapon->aimAt(theOneWhoMustDie->getPosition());
	}

	Weapon* getWeapon() {
		return equipedWeapon;
	}

	virtual ~Player(){};
protected:
private:
};

/**
 * Callback for the player, used to check for input and perform other actions that must be performed once per frame.
 */
class PlayerNodeCallback : public osg::NodeCallback
{
public:
	Player* player;

	PlayerNodeCallback(Player * player) {
		this->player = player;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#ifdef MULTIPLE_PLAYERS
Player * getActivePlayer();
void setActivePlayer(std::string newActivePlayer);
void addNewPlayer(std::string playerName, osg::Vec3 position = osg::Vec3(0,0,5.0f));
#endif

#endif // PLAYER_H
