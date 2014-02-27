#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#define DEFAULT_WEAPON_IMAGE "staff.png"
#define DEFAULT_WEAPON_TYPE "Staff"

#include "Projectile.h"



/// Class for weapons.
class Weapon : public Item
{
protected:
	bool triggerPressed;
	bool firing;
	bool _ready;
	osg::PositionAttitudeTransform* projectileStartingTransform;	/// The position where the projectile will start (if this is a gun, this is the end of the barrel).
	float _coolDownTime;
	float _coolDownTimeRemaining = 0.0;
	std::string _team;	/// The team of the weapon's current owner, used to prevent friendly fire.

	GameObjectData* _projectileData;

public:
	Weapon(GameObjectData* dataObj);
	virtual ~Weapon();

	void fire();
	bool isReady();

	virtual void onUpdate(float deltaTime);

	/// Aims the Weapon at the target world coordinates.
	void aimAt(osg::Vec3 target);
	void setRotation(double angle);

	void setTeam(std::string team);

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveWeaponData(GameObjectData* dataObj);
	void loadWeaponData(GameObjectData* dataObj);
};

//void registerWeaponStats();


#endif // WEAPON_H
