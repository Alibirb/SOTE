#include "Attack.h"

#include "GameObjectData.h"

#include "Fighter.h"

#include "AngelScriptEngine.h"

Attack::Attack()
{
	registerAttack();
	_inUse = false;
}

Attack::Attack(GameObjectData* dataObj) : Attack()
{
	load(dataObj);
}

Attack::~Attack()
{
	//dtor
}

bool Attack::done()
{
	return !_inUse;
}
double Attack::getTimeRemaining()
{
	return _timeRemaining;
}

void Attack::useOn(Fighter* target)
{
	_inUse = true;
	_timeRemaining = _duration;
	//if(target->isHurtByTeam(getTeam()))
		target->takeDamages(getDamages());
}

Damages Attack::getDamages() {
	return _damages;
}

void Attack::onUpdate(float deltaTime)
{
	if(_inUse)
	{
		_timeRemaining -= deltaTime;
		if(_timeRemaining <= 0)
			_inUse = false;
	}
}

GameObjectData* Attack::save()
{
	GameObjectData* dataObj =  new GameObjectData(_objectType);
	saveAttackData(dataObj);
	return dataObj;
}
void Attack::load(GameObjectData* dataObj)
{
	loadAttackData(dataObj);
}


void Attack::saveAttackData(GameObjectData* dataObj)
{
	std::vector<GameObjectData*> damageListData;
	for(Damage damage : _damages)
	{
		GameObjectData* damageData = new GameObjectData("damage");
		damageData->addData("type", damage.type);
		damageData->addData("amount", damage.amount);
		damageListData.push_back(damageData);
	}
	dataObj->addData("damages", damageListData);

	dataObj->addData("duration", _duration);
}
void Attack::loadAttackData(GameObjectData* dataObj)
{
	for(GameObjectData* damageData : dataObj->getObjectList("damages"))
	{
		Damage dam;
		dam.type = damageData->getString("type");
		dam.amount = damageData->getFloat("amount");
		_damages.push_back(dam);
	}

	if(dataObj->hasFloat("duration"))
		_duration = dataObj->getFloat("duration");
}



namespace AngelScriptWrapperFunctions
{
	Attack* AttackFactoryFunction()
	{
		return new Attack();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerAttack()
{
	static bool registered = false;
	if(registered)
		return;


	getScriptEngine()->registerObjectType("Attack", sizeof(Attack), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("Attack", "Attack@ f()", asFUNCTION(AttackFactoryFunction));

	getScriptEngine()->registerObjectMethod("Attack", "bool done()", asMETHOD(Attack, done), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Attack", "double getTimeRemaining()", asMETHOD(Attack, getTimeRemaining), asCALL_THISCALL);
	//getScriptEngine()->registerObjectMethod("Fighter", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);
	//getScriptEngine()->registerObjectMethod("Fighter", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);

	registered = true;
}
