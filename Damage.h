#ifndef DAMAGE_H
#define DAMAGE_H

#include <vector>
#include <string>
/*
enum DamageType
{
	HOT, COLD, SHARP, IMPACT, ACID, PURE_MAGIC, NAP, POISON
};
*/

/// NOTE: this should really be a struct, but it doesn't seem like AngelScript supports structs.
class Damage
{
public:
	//DamageType type;
	std::string type;
	float amount;
};

typedef std::vector<Damage> Damages;

/// Registers Damages, Damage, and DamageType to AngelScript.
void registerDamages();


#endif // DAMAGE_H_
