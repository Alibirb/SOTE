#include "Player.h"
#include "Enemy.h"
#include "Weapon.h"
#include "PhysicsData.h"
#include "Controller.h"


std::string activePlayerName;
std::unordered_map<std::string, Player*> players;	/// List of all Players, identified by their name.



Player::Player(std::string name, osg::Vec3 position) : Fighter(name, position, "Player")
{
	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Player";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	controller->getGhostObject()->setUserPointer(userData);
#endif
}

Player::Player(TiXmlElement* xmlElement) : Fighter(xmlElement)
{
	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Player";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	controller->getGhostObject()->setUserPointer(userData);
#endif
	_team = "Player";
	_equippedWeapon->setTeam(_team);
}

void Player::processMovementControls(osg::Vec3 controlVector)
{
	if(controlVector.length() > 1.0f ) controlVector.normalize();
	osg::Vec3 worldMovement = cameraToWorldTranslation(controlVector * maxSpeed * getDeltaTime());

	double controlAngle =  -atan( worldMovement.y()/worldMovement.x());
	if ( worldMovement.x() < 0 )
	{
		controlAngle += pi;
	}
	static float threshold = pi/12.0;	// For turning, I think. Will need to check previous code to see what this used to mean.
	float angleDistance = (controlAngle - pi/2) - this->getHeading();
	if (abs(angleDistance) > pi)
	{
		if (angleDistance > 0 ) angleDistance -= 2*pi;
		else angleDistance += 2*pi;
	}
	if (worldMovement != osg::Vec3(0,0,0))
	{
		this->setHeading(controlAngle - pi/2);
	}

#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetLinearVelocity(toB2Vec2(worldMovement / getDeltaTime()));
#else
	this->controller->setWalkDirection(osgbCollision::asBtVector3(worldMovement));
	// TODO: should turn around until it reaches the right point, instead of instantly changing.
#endif
}


osg::Vec3 Player::getCameraTarget()
{
	return this->getWorldPosition() + osg::Vec3(0, 0, 1.5);
}

void Player::attack(Enemy *theOneWhoMustDie)
{
	_equippedWeapon->fire();
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

void Player::onCollision(GameObject* other)
{
	if(dynamic_cast<Projectile*>(other))
		onCollision(dynamic_cast<Projectile*>(other));
	else if(dynamic_cast<Controller*>(other))
		onCollision(dynamic_cast<Controller*>(other));
}
void Player::onCollision(Controller* controller)
{
	_willInteractWith = controller;
}

void Player::interact()
{
	if(_willInteractWith)
		_willInteractWith->onPlayerInteraction();
}

Player::~Player()
{
	std::cout << "Player destructor called" << std::endl;
}






Player* getClosestPlayer(osg::Vec3 position)
{
	Player* closest;
	float shortestDistance = FLT_MAX;

	for(auto kv : players)
	{
		Player* p = kv.second;	// Get the Player from the key-value pair.
		if(getDistance(position, p->getWorldPosition()) < shortestDistance)
		{
			closest = p;
			shortestDistance = getDistance(position, p->getWorldPosition());
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
void addPlayer(std::string playerName, Player* thePlayer)
{
	players[playerName] = thePlayer;
}

std::unordered_map<std::string, Player*> getPlayers()
{
	return players;
}
