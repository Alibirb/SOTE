#ifndef DAMAGE_H
#define DAMAGE_H

#include <vector>
#include <string>


/// NOTE: this should really be a struct, but it doesn't seem like AngelScript supports structs.
class Damage
{
public:
	std::string type;
	float amount;
};

typedef std::vector<Damage> Damages;

/// Registers Damages, Damage, and DamageType to AngelScript.
void registerDamages();


#endif // DAMAGE_H_
