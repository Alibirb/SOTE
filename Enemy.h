#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

class Enemy : public Entity
{
protected:
	float health = 10.0;
public:
	Enemy(std::string name, osg::Vec3 position);
	void attack();
	void takeDamage(float amount);
	virtual ~Enemy();

protected:
private:
};


std::list<Enemy*> getEnemies();
void addEnemy(Enemy* newEnemy);


#endif // ENEMY_H
