#include "Player.h"
#include "Enemy.h"
#include "Weapon.h"


std::string activePlayerName;
std::unordered_map<std::string, Player*> players;	/// List of all Players, identified by their name.


Player::Player(std::string name, osg::Vec3 position) : Fighter(name, position, "Player")
{
	Box2DUserData *userData = new Box2DUserData;
	userData->owner = this;
	userData->ownerType = "Player";
	physicsBody->SetUserData(userData);
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

bool Player::isActivePlayer()
{
	return ( activePlayerName == this->name);
}

void Player::die()
{
	this->state = dead;
	if(getPlayers().size() == 1)
	{
		// Last Player. Game Over
		GameOverYouLose();
	}
	else
		logError("Not implemented.");
}

void Player::onUpdate(float deltaTime)
{

}

Player::~Player()
{
	std::cout << "Player destructor called" << std::endl;
}




Player* getClosestPlayer(osg::Vec3 position)
{
	Player* closest;
	float shortestDistance = 9999999999999999999;

	for(auto kv : players)
	{
		Player* p = kv.second;	// Get the Player from the key-value pair.
		if(getDistance(position, p->getPosition()) < shortestDistance)
		{
			closest = p;
			shortestDistance = getDistance(position, p->getPosition());
		}
	}

	return closest;
}

Player* getActivePlayer()
{
	return players[activePlayerName];
}

void setActivePlayer(std::string newActivePlayerName)
{
	activePlayerName = newActivePlayerName;
}

void addNewPlayer(std::string playerName, osg::Vec3 position)
{
	players[playerName] = new Player(playerName, position);
}

std::unordered_map<std::string, Player*> getPlayers()
{
	return players;
}
