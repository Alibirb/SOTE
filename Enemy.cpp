#include "Enemy.h"

std::list<Enemy*> enemyList;


Enemy::Enemy(std::string name, osg::Vec3 position) : Entity(name, position)
{
	addEnemy(this);	// put this Enemy in the enemyList.
}

Enemy::~Enemy()
{
	enemyList.remove(this);	// This Enemy is being destroyed. Remove it from the list to prevent null pointer.
}

void Enemy::attack()
{

}

void Enemy::takeDamage(float amount)
{
	this->health -= amount;
	if (health <= 0.0)
	{
		this->state = dead;
		//markEntityForRemoval(this);	// Cannot call delete on itself, so must flag for deletion instead.
		std::cout << this->name << " has died" << std::endl;
		delete this;
	}
}



std::list<Enemy*> getEnemies()
{
	return enemyList;
}
void addEnemy(Enemy* newEnemy)
{
	enemyList.push_back(newEnemy);
}
