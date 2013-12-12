#include "Player.h"
#include "Enemy.h"
#include "Weapon.h"


std::string activePlayerName;
std::map<std::string, Player*> playerList;


Player::Player(std::string name, osg::Vec3 position) : Entity(name, position, DEFAULT_PLAYER_IMAGE)
{
	modelNode->setUpdateCallback(new PlayerNodeCallback(this));
	equipWeapon(new Weapon());
	transformNode->addChild(equipedWeapon->getTransformNode());
}

void Player::setPosition(osg::Vec3 newPosition)
{
	Entity::setPosition(newPosition);
}

void Player::processMovementControls(osg::Vec3 controlVector)
{
	osg::Vec3 movementVector = controlVector * maxSpeed * getDeltaTime();

	//this->setPosition(getPosition() + movementVector);

	physicsBody->SetLinearVelocity(maxSpeed * toB2Vec2(controlVector));
}


osg::Vec3 Player::getCameraTarget()
{
	return this->getPosition();
}

void Player::attack(Enemy *theOneWhoMustDie)
{
	equipedWeapon->fire();
}

void Player::equipWeapon(Weapon *theWeapon)
{
	equipedWeapon = theWeapon;
}

void Player::aimWeapon(Enemy *theOneWhoMustDie)
{
	equipedWeapon->aimAt(theOneWhoMustDie->getPosition());
}

Weapon* Player::getWeapon()
{
	return equipedWeapon;
}

bool Player::isActivePlayer()
{
	return ( activePlayerName == this->name);
}



void PlayerNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (_player->isActivePlayer())
	{

	}

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}




Player* getActivePlayer()
{
	return playerList[activePlayerName];
}

void setActivePlayer(std::string newActivePlayerName)
{
	activePlayerName = newActivePlayerName;
}

void addNewPlayer(std::string playerName, osg::Vec3 position)
{
	playerList[playerName] = new Player(playerName, position);
}
