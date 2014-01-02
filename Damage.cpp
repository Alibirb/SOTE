#include "Damage.h"

#include "AngelScriptEngine.h"
#include "stdvector.h"

/*
namespace AngelScriptWrapperFunctions
{
	void DamageConstructor(Damage *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) Damage();
	}
	void DamageInitConstructor(DamageType type, float amount, Damage *self)
	{
		new(self) Damage(type, amount);
	}
	void DamageCopyConstructor(Damage& other, Damage* self)
	{
		new(self) Damage(other.type, other.amount);
	}
	void DamageDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((Damage*)memory)->~Damage();
	}
}

using namespace AngelScriptWrapperFunctions;


Damage::Damage()
{
}

Damage::Damage(DamageType type, float amount)
{
	this->type = type;
	this->amount = amount;
}
*/

void registerDamageType()
{
	getScriptEngine()->registerEnum("DamageType", "HOT", HOT, "COLD", COLD, "SHARP", SHARP, "IMPACT", IMPACT, "ACID", ACID);
}

void registerDamage()
{
	registerDamageType();
	getScriptEngine()->registerObjectType("Damage", sizeof(Damage), asOBJ_VALUE | asOBJ_POD | GetTypeTraits<Damage>());
	//getScriptEngine()->registerConstructor("Damage", "void f(DamageType, float)", asFUNCTION(DamageInitConstructor));
	//getScriptEngine()->registerConstructor("Damage", "void f(const DamageType &in)", asFUNCTION(DamageCopyConstructor));
	//getScriptEngine()->registerDestructor("Damage", asFUNCTION(DamageDestructor));
	getScriptEngine()->registerObjectProperty("Damage", "DamageType type", asOFFSET(Damage, type));
	getScriptEngine()->registerObjectProperty("Damage", "float amount", asOFFSET(Damage, amount));
}

void registerDamages()
{
	static bool registered = false;
	if(registered)
		return;	// Already registered

	registerDamage();
	//getScriptEngine()->registerVector<Damage>("vector<Damage>", "Damage");
	getScriptEngine()->registerVector<Damage>("Damages", "Damage");
	//RegisterVector<int>("vector<Damage>", "Damage", getScriptEngine()->getInternalEngine());

	registered = true;
}
