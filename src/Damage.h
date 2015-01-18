#ifndef DAMAGE_H
#define DAMAGE_H

#include <vector>
#include <string>

#include "GameObjectData.h"

/// Class representing damage of some kind.
class Damage : public Saveable
{
public:
	std::string type;
	float amount;

	GameObjectData* save();
	void load(GameObjectData* dataObj);
};

typedef std::vector<Damage> Damages;

/// Registers Damages, Damage, and DamageType to AngelScript.
void registerDamages();


#endif // DAMAGE_H_
