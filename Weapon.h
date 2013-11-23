#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#include "Projectile.h"

/// Class for weapons.
class Weapon : public Item
{
protected:
	bool triggerPressed;
	bool firing;
	osg::Vec3 target;
	osg::Vec3 position;
public:
	Weapon();
	virtual ~Weapon();

	void fire() {
		new Projectile(this->position, target - position);
	}
	void aimAt(osg::Vec3 target) {
		this->target = target;
	}
	void setPosition(osg::Vec3 position) {
		this->position = position;
	}


protected:
private:
};



#endif // WEAPON_H
