#ifndef FIGHTER_H
#define FIGHTER_H

#include "Entity.h"
#include "Damage.h"


#include "Weapon.h"


class FighterStats
{
public:
	FighterStats();
	float maxHealth;
	std::map<DamageType, float> resistances;
	std::string imageFilename;
	std::string weaponType;
	WeaponStats weaponStats;

	/// For scripting use (std::map is not exposed)
	float getResistance(DamageType& damType);
	void setResistance(DamageType& type, float value);

	void setWeaponStats(WeaponStats& stats)
	{
		this->weaponStats = stats;
	}
};


class Fighter : public Entity
{
protected:
	Weapon *equipedWeapon;
	float health = 10.0;
	FighterStats _stats;

public:
	Fighter(std::string name, osg::Vec3 position);
	virtual ~Fighter();
	void equipWeapon(Weapon *theWeapon);

	void aimWeapon(Entity *theOneWhoMustDie);

	Weapon* getWeapon();

	virtual void loadStats(std::string scriptFilename);

	void setStats(FighterStats& newStats);

	virtual void takeDamages(Damages dams);

	float getResistance(DamageType& type);

	/// Perform any actions to be taken when the Fighter is killed (mark for removal, change state to "dead", etc.)
	virtual void die()=0;
protected:
private:
};

void addDamageIndicator(Fighter* entityHurt, float damageDealt, DamageType& damageType);
void registerFighterStats();

#endif // FIGHTER_H
