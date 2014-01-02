#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"

std::list<Enemy*> enemyList;


Enemy::Enemy(std::string name, osg::Vec3 position) : Fighter(name, position)
{
	addEnemy(this);	// put this Enemy in the enemyList.
	Box2DUserData *userData = new Box2DUserData;
	userData->owner = this;
	userData->ownerType = "Enemy";
	physicsBody->SetUserData(userData);
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

	aimWeapon(getClosestPlayer(this->getPosition()));
	if(equipedWeapon->isReady())
	{
		equipedWeapon->fire();
	}
}



Enemy* getClosestEnemy(osg::Vec3 position, std::list<Enemy*> possibilities)
{
	Enemy* closest;
	float shortestDistance = 9999999999999999999;
	for(Enemy* enemy : possibilities)
		if(getDistance(position, enemy->getPosition()) < shortestDistance)
		{
			closest = enemy;
			shortestDistance = getDistance(position, enemy->getPosition());
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
