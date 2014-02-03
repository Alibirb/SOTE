#ifndef FIGHTER_H
#define FIGHTER_H

#include "Entity.h"
#include "Damage.h"


#include "Weapon.h"

class TiXmlElement;


class FighterStats
{
public:
	FighterStats();
	float maxHealth;
	//std::map<DamageType, float> resistances;
	std::map<std::string, float> resistances;
	std::string modelFilename;
	std::string weaponType;
	WeaponStats weaponStats;

	/// For scripting use (std::map is not exposed)
//	float getResistance(DamageType& damType);
//	void setResistance(DamageType& type, float value);
	float getResistance(std::string& damType);
	void setResistance(std::string& type, float value);

	void setWeaponStats(WeaponStats& stats) {
		this->weaponStats = stats;
	}

};


class Fighter : public Entity
{
protected:
	Weapon *_equippedWeapon;
	float health = 10.0;
	FighterStats _stats;
	std::string _team;	/// The team of the weapon's current owner, used to prevent friendly fire.

public:
	Fighter(std::string name, osg::Vec3 position, std::string team);
	Fighter(TiXmlElement* xmlElement);
	virtual ~Fighter();
	void equipWeapon(Weapon *theWeapon);
	void unequipWeapon();

	void aimWeapon(Entity *theOneWhoMustDie);

	Weapon* getWeapon();

	virtual void loadStats(std::string scriptFilename);

	void setStats(FighterStats& newStats);

	void load(std::string xmlFilename);
	void load(TiXmlElement* xmlElement);

	virtual void takeDamages(Damages dams);

	//float getResistance(DamageType& type);
	float getResistance(std::string type);
	void setResistance(std::string type, float value);

	/// Perform any actions to be taken when the Fighter is killed (mark for removal, change state to "dead", etc.)
	virtual void die()=0;

	bool isHurtByTeam(std::string otherTeam);

	virtual void onCollision(GameObject* other);
	virtual void onCollision(Projectile* projectile);

protected:
};

//void addDamageIndicator(Fighter* entityHurt, float damageDealt, DamageType& damageType);
void addDamageIndicator(Fighter* entityHurt, float damageDealt, std::string& damageType);
void registerFighterStats();

#endif // FIGHTER_H
