#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#include "Projectile.h"

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

	void fire() {
		//new Projectile(this->getWorldPosition(), target - getWorldPosition());
		new Projectile(getWorldCoordinates(projectileStartingTransform), target - getWorldCoordinates(projectileStartingTransform));
	}

	/// Aims the Weapon at the target coordinates (specified in world coordinates)
	void aimAt(osg::Vec3 target)
	{
		osg::Vec3 position = this->getWorldPosition();
		osg::Vec3 diff = target - position;
		osg::Quat rotation;

		//rotation.makeRotate(position, target);

		float angle = atand(diff.y()/diff.x());
		if (diff.x() < 0)
			angle = 180 - angle;
		if (angle < 0)
			angle += 360;
		rotation.makeRotate(angle, osg::Vec3(0,0,1));

		transformNode->setAttitude(rotation);

		this->target = target;
	}
	void setPosition(osg::Vec3 position) {
		this->transformNode->setPosition(position);
	}
	osg::Vec3 getPosition() {
		return transformNode->getPosition();
	}
	osg::Vec3 getWorldPosition() {
		//return transformNode->getPosition() + ( (osg::PositionAttitudeTransform*) (transformNode->getParent(0)))->getPosition();
		return getWorldCoordinates(transformNode);
	}


protected:
private:
};



#endif // WEAPON_H
