#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#define DEFAULT_WEAPON_IMAGE "staff.png"
#define DEFAULT_WEAPON_TYPE "Staff"

class WeaponStats
{
public:
	WeaponStats();
	WeaponStats(std::string imageFilename, std::string projectileType, float coolDownTime);
	std::string projectileType;
	std::string imageFilename;
	float coolDownTime;
};

/// Class for weapons.
class Weapon : public Item
{
protected:
	bool triggerPressed;
	bool firing;
	bool _ready;
	osg::PositionAttitudeTransform* projectileStartingTransform;	/// The position where the projectile will start (if this is a gun, this is the end of the barrel).
	//float _coolDownTime = 2.5;	/// Time it takes to cool down between firing.
	float _coolDownTimeRemaining = 0.0;
	WeaponStats _stats;

public:
	Weapon(std::string type = DEFAULT_WEAPON_TYPE);
	virtual ~Weapon();

	void fire();
	bool isReady();

	void onUpdate(float deltaTime);

	/// Aims the Weapon at the target world coordinates.
	void aimAt(osg::Vec3 target);
	void setPosition(osg::Vec3 position);
	osg::Vec3 getPosition();
	osg::Vec3 getWorldPosition();
	void setRotation(double angle);
	void loadStats(std::string scriptFilename);
	void setStats(WeaponStats& stats)
	{
		this->_stats = stats;
	}

protected:
private:
};

void registerWeaponStats();


#endif // WEAPON_H
