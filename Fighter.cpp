#include "Fighter.h"
#include "Weapon.h"

Fighter::Fighter(std::string name, osg::Vec3 position, std::string imageFilename) : Entity(name, position, imageFilename)
{
	equipWeapon(new Weapon());
	transformNode->addChild(equipedWeapon->getTransformNode());
}

Fighter::~Fighter()
{
	delete equipedWeapon;	// Note that this may not be the final desired behavior.
}

void Fighter::equipWeapon(Weapon *theWeapon)
{
	equipedWeapon = theWeapon;
}

void Fighter::aimWeapon(Entity *theOneWhoMustDie)
{
	equipedWeapon->aimAt(theOneWhoMustDie->getPosition());
}

Weapon* Fighter::getWeapon()
{
	return equipedWeapon;
}

void Fighter::takeDamage(float amount)
{
	if(this->state == dead)
		return;
	this->health -= amount;
	if (health <= 0.0)
	{
		this->die();
	}
}
