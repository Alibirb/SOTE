#include "Damage.h"

#include "AngelScriptEngine.h"
#include "stdvector.h"


void registerDamage()
{
	getScriptEngine()->registerObjectType("Damage", sizeof(Damage), asOBJ_VALUE | asOBJ_POD | GetTypeTraits<Damage>());
	getScriptEngine()->registerObjectProperty("Damage", "string type", asOFFSET(Damage, type));
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
