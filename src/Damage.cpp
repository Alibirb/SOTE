#include "Damage.h"

#include "AngelScriptEngine.h"
#include "stdvector.h"


GameObjectData* Damage::save()
{
	GameObjectData* dataObj = new GameObjectData();
	saveSaveableVariables(dataObj);

	dataObj->addData("type", type);
	dataObj->addData("amount", amount);

	return dataObj;
}

void Damage::load(GameObjectData* dataObj)
{
	loadSaveableVariables(dataObj);
	type = dataObj->getString("type");
	amount = dataObj->getFloat("amount");
}

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
