#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#define DEFAULT_WEAPON_IMAGE "staff.png"

/// Class for weapons.
class Weapon : public Item
{
protected:
	bool triggerPressed;
	bool firing;
	osg::Vec3 target;
	osg::PositionAttitudeTransform* projectileStartingTransform;	/// The position where the projectile will start (if this is a gun, this is the end of the barrel).

public:
	Weapon();
	virtual ~Weapon();

	void fire();

	/// Aims the Weapon at the target coordinates (specified in world coordinates)
	void aimAt(osg::Vec3 target);
	void setPosition(osg::Vec3 position);
	osg::Vec3 getPosition();
	osg::Vec3 getWorldPosition();
	void setRotation(double angle);

protected:
private:
};



#endif // WEAPON_H
