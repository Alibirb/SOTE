#ifndef ATTACK_H
#define ATTACK_H

#include "Damage.h"

#include "GameObjectData.h"

class Fighter;


class GameObjectData;

/// Class for attacks.
/// TODO: should inherit from a class called "Action"
class Attack : public Saveable
{
protected:
	Damages _damages;
	double _duration;	/// how long the attack takes
	bool _inUse;

	std::string _objectType = "Attack";

	///TODO:
	/// something to mask the effect by team.

	double _timeRemaining;

public:
	Attack();
	Attack(GameObjectData* dataObj);
	virtual ~Attack();

	Damages getDamages();

	virtual void useOn(Fighter* target);

	virtual void onUpdate(float deltaTime);

	bool done();
	double getTimeRemaining();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveAttackData(GameObjectData* dataObj);
	void loadAttackData(GameObjectData* dataObj);

};


void registerAttack();

#endif // ATTACK_H
