#ifndef ENEMY_H
#define ENEMY_H

#include "Fighter.h"

#define DEFAULT_ENEMY_IMAGE "enemy.png"

class Enemy : public Fighter
{
protected:

public:
	Enemy(std::string name, osg::Vec3 position);
	Enemy(TiXmlElement* xmlElement);
	void attack();
	void die();
	virtual ~Enemy();
	virtual void onUpdate(float deltaTime);

protected:
};


std::list<Enemy*> getEnemies();
void addEnemy(Enemy* newEnemy);

/// Returns the closest Enemy to the specified position
Enemy* getClosestEnemy(osg::Vec3 position, std::list<Enemy*> possibilities = getEnemies());


#endif // ENEMY_H
