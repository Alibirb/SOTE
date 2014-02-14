#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "PhysicsData.h"

std::list<Enemy*> enemyList;


Enemy::Enemy(std::string name, osg::Vec3 position) : Fighter(name, position, "Enemy")
{
	_objectType = "enemy";

	addEnemy(this);	// put this Enemy in the enemyList.

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Enemy";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	controller->getGhostObject()->setUserPointer(userData);
#endif
}
Enemy::Enemy(XMLElement* xmlElement) : Fighter(xmlElement)
{
	_objectType = "enemy";

	addEnemy(this);	// put this Enemy in the enemyList.

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Enemy";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	controller->getGhostObject()->setUserPointer(userData);
#endif
	_team = "Enemy";
	_equippedWeapon->setTeam(_team);
}

Enemy::~Enemy()
{
	enemyList.remove(this);	// This Enemy is being destroyed. Remove it from the list to prevent null pointer.
}

void Enemy::attack()
{

}

void Enemy::die()
{
	this->state = dead;
	markForRemoval(this, "Enemy");	// this may not be a safe time to delete the enemy (for instance, if we're in the middle of running physics), so simply mark this for deletion at a safer time.
	std::cout << this->name << " has died" << std::endl;
}

void Enemy::onUpdate(float deltaTime)
{
	if(this->state == dead)
		return;

	aimWeapon(getClosestPlayer(this->getWorldPosition()));
	if(_equippedWeapon->isReady())
	{
		_equippedWeapon->fire();
	}
}



Enemy* getClosestEnemy(osg::Vec3 position, std::list<Enemy*> possibilities)
{
	Enemy* closest;
	float shortestDistance = FLT_MAX;
	for(Enemy* enemy : possibilities)
		if(getDistance(position, enemy->getWorldPosition()) < shortestDistance)
		{
			closest = enemy;
			shortestDistance = getDistance(position, enemy->getWorldPosition());
		}

	return closest;
}

std::list<Enemy*> getEnemies()
{
	return enemyList;
}
void addEnemy(Enemy* newEnemy)
{
	enemyList.push_back(newEnemy);
}
