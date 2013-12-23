#ifndef FIGHTER_H
#define FIGHTER_H

#include "Entity.h"


class Fighter : public Entity
{
protected:
	Weapon *equipedWeapon;
	float health = 10.0;
public:
	Fighter(std::string name, osg::Vec3 position, std::string imageFilename = DEFAULT_ENTITY_IMAGE);
	virtual ~Fighter();
	void equipWeapon(Weapon *theWeapon);

	void aimWeapon(Entity *theOneWhoMustDie);

	Weapon* getWeapon();

	virtual void takeDamage(float amount);

	/// Perform any actions to be taken when the Fighter is killed (mark for removal, change state to "dead", etc.)
	virtual void die()=0;
protected:
private:
};

#endif // FIGHTER_H
