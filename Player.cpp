#include "Player.h"
#include "Weapon.h"
#include "PhysicsData.h"
#include "Controller.h"

#include "Level.h"


/*
Player::Player(std::string name, osg::Vec3 position) : Fighter(name, position, "Player") , _willInteractWith(NULL)
{
	_objectType = "player";

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Player";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_controller->getGhostObject()->setUserPointer(userData);
#endif
}*/

Player::Player(GameObjectData* dataObj, osg::Group* parentNode) : Fighter(dataObj, parentNode) , _willInteractWith(NULL)
{
	_objectType = "Player";

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Player";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	_controller->getGhostObject()->setUserPointer(userData);
#endif
	_team = "Player";
	if(_equippedWeapon)
		_equippedWeapon->setTeam(_team);
}

void Player::processMovementControls(osg::Vec3 controlVector)
{
	if(controlVector.length() > 1.0f ) controlVector.normalize();
	osg::Vec3 worldMovement = cameraToWorldTranslation(controlVector * _maxSpeed * getDeltaTime());

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
	_controller->setWalkDirection(osgbCollision::asBtVector3(worldMovement));
	// TODO: should turn around until it reaches the right point, instead of instantly changing.
#endif
}


osg::Vec3 Player::getCameraTarget()
{
	return this->getWorldPosition() + osg::Vec3(0, 0, 1.5);
}
/*
void Player::attack(Fighter *theOneWhoMustDie)
{
	_equippedWeapon->fire();
}*/

bool Player::isActivePlayer()
{
	return (getCurrentLevel()->getActivePlayer() == this);
}

void Player::die()
{
//	this->state = dead;
	if(getPlayers().size() == 1)
	{
		// Last Player. Game Over
		GameOverYouLose();
	}
	else
		logError("Player death not implemented.");
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
	/// TODO: should check whether it's closer to this Controller than it is to current Controller
	_willInteractWith = controller;
}

void Player::interact()
{
	if(_willInteractWith)
		_willInteractWith->onPlayerInteraction();
}

Player::~Player()
{

}






Player* getClosestPlayer(osg::Vec3 position)
{
	return getCurrentLevel()->getClosestPlayer(position);
}

Player* getActivePlayer()
{
	return getCurrentLevel()->getActivePlayer();
}

void setActivePlayer(std::string newActivePlayerName)
{
	getCurrentLevel()->setActivePlayer(newActivePlayerName);
}

void addPlayer(std::string playerName, Player* thePlayer)
{
	getCurrentLevel()->addPlayer(playerName, thePlayer);
}

std::list<std::string> getPlayerNames()
{
	return getCurrentLevel()->getPlayerNames();
}

std::unordered_map<std::string, Player*> getPlayers()
{
	return getCurrentLevel()->getPlayers();
}



