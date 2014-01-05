#include "Damage.h"

#include "AngelScriptEngine.h"
#include "stdvector.h"


void registerDamageType()
{
	getScriptEngine()->registerEnum("DamageType", "HOT", HOT, "COLD", COLD, "SHARP", SHARP, "IMPACT", IMPACT, "ACID", ACID, "PURE_MAGIC", PURE_MAGIC, "NAP", NAP, "POISON", POISON);
}

void registerDamage()
{
	registerDamageType();
	getScriptEngine()->registerObjectType("Damage", sizeof(Damage), asOBJ_VALUE | asOBJ_POD | GetTypeTraits<Damage>());
	getScriptEngine()->registerObjectProperty("Damage", "DamageType type", asOFFSET(Damage, type));
	getScriptEngine()->registerObjectProperty("Damage", "float amount", asOFFSET(Damage, amount));
}

void registerDamages()
{
	static bool registered = false;
	if(registered)
		return;	// Already registered

	registerDamage();
	getScriptEngine()->registerVector<Damage>("Damages", "Damage");

	registered = true;
}
