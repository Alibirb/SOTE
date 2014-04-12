#ifndef DAMAGE_H
#define DAMAGE_H

#include <vector>
#include <string>


/// Class representing damage of some kind.
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
