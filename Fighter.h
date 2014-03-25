#ifndef FIGHTER_H
#define FIGHTER_H

#include "Entity.h"
#include "Damage.h"


#include "Weapon.h"
#include "Attack.h"


class Fighter;
std::list<Fighter*> getFighters();

class Fighter : public Entity
{
protected:
	Weapon* _equippedWeapon;
	float health = 10.0;
	std::string _team;
	std::vector<Attack*> _attacks;
	Attack* _currentAttack;	/// currently-selected attack

	std::unordered_map<std::string, float> _resistances;
	float _maxHealth;

public:
	Fighter();
	Fighter(std::string name, osg::Vec3 position, std::string team);
	Fighter(GameObjectData* dataObj);
	virtual ~Fighter();
	void equipWeapon(Weapon* theWeapon);
	void unequipWeapon();
	void aimWeapon(Entity* theOneWhoMustDie);
	Weapon* getWeapon();

	Attack* getCurrentAttack();

	virtual void takeDamages(Damages dams);

	float getResistance(std::string type);
	void setResistance(std::string type, float value);

	virtual void attack(Fighter* target);
	virtual void useBestAttackOn(Fighter* target);

	virtual void die();	/// Perform any actions to be taken when the Fighter is killed (mark for removal, change state to "dead", etc.)

	bool isHurtByTeam(std::string otherTeam);
	bool isEnemyOf(Fighter* other);
	bool isAllyOf(Fighter* other);
	std::string getTeam();

	virtual void onCollision(GameObject* other);
	virtual void onCollision(Projectile* projectile);

	virtual void onUpdate(float deltaTime);

	std::list<Fighter*> getEnemies();
	std::list<Fighter*> getAllies();
	Fighter* getClosestEnemy(std::list<Fighter*> possibilities = getFighters());
	Fighter* getClosestAlly(std::list<Fighter*> possibilities = getFighters());


	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveFighterData(GameObjectData* dataObj);
	void loadFighterData(GameObjectData* dataObj);


};

void addDamageIndicator(Fighter* entityHurt, float damageDealt, std::string& damageType);

void addFighter(Fighter* newFighter);

void registerFighter();

#endif // FIGHTER_H
