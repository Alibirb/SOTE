#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"
#define DEFAULT_WEAPON_IMAGE "staff.png"
#define DEFAULT_WEAPON_TYPE "Staff"

#include "Projectile.h"

class WeaponStats
{
public:
	std::string projectileType;
	ProjectileStats projectileStats;
	std::string imageFilename;
	float coolDownTime;

	WeaponStats();
	WeaponStats(std::string imageFilename, std::string projectileType, ProjectileStats& projectileStats, float coolDownTime);
	static WeaponStats loadPrototype(std::string prototypeName);	/// Loads prototype stats for the specified type
	void setProjectileStats(ProjectileStats& stats)
	{
		projectileStats.imageFilename = stats.imageFilename;
		projectileStats.damages = stats.damages;
	}
};

/// Class for weapons.
class Weapon : public Item
{
protected:
	bool triggerPressed;
	bool firing;
	bool _ready;
	osg::PositionAttitudeTransform* projectileStartingTransform;	/// The position where the projectile will start (if this is a gun, this is the end of the barrel).
	float _coolDownTimeRemaining = 0.0;
	WeaponStats _stats;
	std::string _team;	/// The team of the weapon's current owner, used to prevent friendly fire.

public:
	Weapon(WeaponStats stats);
	Weapon(std::string type = DEFAULT_WEAPON_TYPE);
	Weapon(XMLElement* xmlElement);
	Weapon(GameObjectData* dataObj);
	virtual ~Weapon();

	void load(XMLElement* xmlElement);
	void load(std::string xmlFilename);

	void fire();
	bool isReady();

	virtual void onUpdate(float deltaTime);

	/// Aims the Weapon at the target world coordinates.
	void aimAt(osg::Vec3 target);
	void setRotation(double angle);
	void setStats(WeaponStats& stats)
	{
		this->_stats = stats;
	}
	void setProjectileStats(ProjectileStats& stats)
	{
		this->_stats.setProjectileStats(stats);
	}

	void setTeam(std::string team)
	{
		this->_team = team;
	}

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);
protected:
	void saveWeaponData(GameObjectData* dataObj);
	void loadWeaponData(GameObjectData* dataObj);
};

void registerWeaponStats();


#endif // WEAPON_H
