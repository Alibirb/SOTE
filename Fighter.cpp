#include "Fighter.h"
#include "Weapon.h"
#include "AngelScriptEngine.h"
#include "GameObjectData.h"
#include "PhysicsData.h"

#include "TemporaryText.h"


std::list<Fighter*> fighterList;

Fighter::Fighter()
{
	registerFighter();
}

Fighter::Fighter(std::string name, osg::Vec3 position, std::string team) : Entity(name, position)
{
	registerFighter();
	this->_team = team;
}
Fighter::Fighter(GameObjectData* dataObj) : Fighter()
{
	_equippedWeapon = NULL;
	_objectType = "Fighter";
	load(dataObj);
	addFighter(this);

	PhysicsUserData *userData = new PhysicsUserData;
	userData->owner = this;
	userData->ownerType = "Fighter";
#ifdef USE_BOX2D_PHYSICS
	physicsBody->SetUserData(userData);
#else
	controller->getGhostObject()->setUserPointer(userData);
#endif
	if(_equippedWeapon)
		_equippedWeapon->setTeam(_team);
}



Fighter::~Fighter()
{
	delete _equippedWeapon;
	fighterList.remove(this);
}

void Fighter::die()
{
//	this->state = dead;
	markForRemoval(this, "Fighter");	// this may not be a safe time to delete the object (for instance, if we're in the middle of running physics), so simply mark this for deletion at a safer time.
	std::cout << this->name << " has died" << std::endl;
}

void Fighter::equipWeapon(Weapon *theWeapon)
{
	if(_equippedWeapon)
		unequipWeapon();
	_equippedWeapon = theWeapon;
	_equippedWeapon->parentTo(_transformNode);
	_equippedWeapon->setPosition(osg::Vec3(0,0,1));
	_equippedWeapon->setTeam(_team);
}
void Fighter::unequipWeapon()
{
	_equippedWeapon->unparentFrom(_transformNode);
}

void Fighter::aimWeapon(Entity *theOneWhoMustDie)
{
	_equippedWeapon->aimAt(theOneWhoMustDie->getWorldPosition());
}

Weapon* Fighter::getWeapon()
{
	return _equippedWeapon;
}

Attack* Fighter::getCurrentAttack()
{
	return _currentAttack;
}

void Fighter::attack(Fighter* target)
{
	_stateMachine->changeState("attacking");
	_currentAttack->useOn(target);
}

void Fighter::useBestAttackOn(Fighter* target)
{
	// TODO: actually check which attack is best.
	if(target && _attacks.front())
		_currentAttack = _attacks.front();
	attack(target);
}

void Fighter::takeDamages(Damages dams)
{
//	if(this->state == dead)
//		return;
	for(Damage dam : dams)
	{
		if(dam.amount * (1.0 - this->getResistance(dam.type) ) > 0)
			addDamageIndicator(this, dam.amount * (1.0 - this->getResistance(dam.type) ), dam.type);
		this->health -= dam.amount * (1.0 - this->getResistance(dam.type) );
		if (health <= 0.0)
		{
			this->die();
		}
	}
}

float Fighter::getResistance(std::string type)
{
	return _resistances[type];
}

void Fighter::setResistance(std::string type, float value)
{
	_resistances[type] = value;
}

bool Fighter::isHurtByTeam(std::string otherTeam)
{
	return (_team.compare(otherTeam) != 0);
}
bool Fighter::isEnemyOf(Fighter* other)
{
	return (other->getTeam() != _team);
}
bool Fighter::isAllyOf(Fighter* other)
{
	return (other->getTeam() == _team);
}
std::string Fighter::getTeam()
{
	return _team;
}

void Fighter::onCollision(Projectile* projectile)
{
	if(isHurtByTeam(projectile->getTeam()))
		takeDamages(projectile->getDamages());
}
void Fighter::onCollision(GameObject* other)
{
	if(dynamic_cast<Projectile*>(other))
		onCollision(dynamic_cast<Projectile*>(other));
}

void Fighter::onUpdate(float deltaTime)
{
//	if(this->state == dead)
//		return;

	/*aimWeapon(getClosestEnemy());
	if(_equippedWeapon->isReady())
	{
		//_equippedWeapon->fire();
		//if(getClosestEnemy() && _attacks.front())
		//	_attacks.front()->useOn(getClosestEnemy());
		if(getClosestEnemy())
			useBestAttackOn(getClosestEnemy());
	}*/
	_stateMachine->onUpdate(deltaTime);
	if(_currentAttack)
		_currentAttack->onUpdate(deltaTime);
}

GameObjectData* Fighter::save()
{
	GameObjectData* dataObj = new GameObjectData(_objectType);

	saveGameObjectVariables(dataObj);
	saveEntityVariables(dataObj);
	saveFighterData(dataObj);

	return dataObj;
}
void Fighter::saveFighterData(GameObjectData* dataObj)
{
	dataObj->addData("team", _team);
	if(getWeapon())
		dataObj->addData("weapon", getWeapon()->save());
	dataObj->addData("maxHealth", _maxHealth);

	GameObjectData* resistanceData = new GameObjectData("unordered_map");
	for(auto kv : _resistances)
	{
		resistanceData->addData(kv.first, kv.second);
	}
	dataObj->addData("resistances", resistanceData);


	std::vector<GameObjectData*> attackData;
	for(Attack* attack : _attacks)
	{
		attackData.push_back(attack->save());
	}
	dataObj->addData("attacks", attackData);
}

void Fighter::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadEntityVariables(dataObj);
	loadFighterData(dataObj);
}
void Fighter::loadFighterData(GameObjectData* dataObj)
{
	_team = dataObj->getString("team");
	_maxHealth = dataObj->getFloat("maxHealth");

	if(dataObj->getObject("weapon"))
		equipWeapon(new Weapon(dataObj->getObject("weapon")));
	if(dataObj->getObject("resistances"))
	{
		_resistances = std::unordered_map<std::string, float>(dataObj->getObject("resistances")->getAllFloats());
	}

	if(!dataObj->getObjectList("attacks").empty())
	{
		for(auto data : dataObj->getObjectList("attacks"))
			_attacks.push_back(new Attack(data));
	}

}

Fighter* Fighter::getClosestEnemy(std::list<Fighter*> possibilities)
{
	Fighter* closest = NULL;
	float shortestDistance = FLT_MAX;
	for(Fighter* fighter : possibilities)
	{
		if(!fighter->isEnemyOf(this))
			continue;
		if(getDistance(this->getWorldPosition(), fighter->getWorldPosition()) < shortestDistance)
		{
			closest = fighter;
			shortestDistance = getDistance(this->getWorldPosition(), fighter->getWorldPosition());
		}
	}

	return closest;
}
Fighter* Fighter::getClosestAlly(std::list<Fighter*> possibilities)
{
	Fighter* closest = NULL;
	float shortestDistance = FLT_MAX;
	for(Fighter* fighter : possibilities)
	{
		if(!fighter->isAllyOf(this))
			continue;
		if(getDistance(this->getWorldPosition(), fighter->getWorldPosition()) < shortestDistance)
		{
			closest = fighter;
			shortestDistance = getDistance(this->getWorldPosition(), fighter->getWorldPosition());
		}
	}

	return closest;
}


void addDamageIndicator(Fighter* entityHurt, float damageDealt, std::string& damageType)
{
	std::ostringstream stream;
	stream << "-" << damageDealt;
	new TemporaryText(stream.str(), entityHurt->getWorldPosition(), 2.5);
}




std::list<Fighter*> getFighters()
{
	return fighterList;
}
void addFighter(Fighter* newFighter)
{
	fighterList.push_back(newFighter);
}



/*
namespace AngelScriptWrapperFunctions
{
	void FighterStatsConstructor(FighterStats *memory)
	{
		// Initialize the pre-allocated memory by calling the object constructor with the placement-new operator
		new(memory) FighterStats();
	}
	void FighterStatsCopyConstructor(FighterStats& other, FighterStats* self)
	{
		new(self) FighterStats();
		self->resistances = other.resistances;
		self->modelFilename = other.modelFilename;
		self->maxHealth = other.maxHealth;
		self->weaponType = other.weaponType;
		self->weaponStats = other.weaponStats;
	}
	void FighterStatsDestructor(void *memory)
	{
		// Uninitialize the memory by calling the object destructor
		((FighterStats*)memory)->~FighterStats();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerFighterStats()
{
	static bool registered = false;
	if(registered)
		return;

	registerDamages();
	registerWeaponStats();

	getScriptEngine()->registerObjectType("FighterStats", sizeof(FighterStats), asOBJ_VALUE | GetTypeTraits<FighterStats>() );
	getScriptEngine()->registerConstructor("FighterStats", "void f()", asFUNCTION(FighterStatsConstructor));
	getScriptEngine()->registerDestructor("FighterStats", asFUNCTION(FighterStatsDestructor));
	getScriptEngine()->registerConstructor("FighterStats", "void f(const FighterStats &in)", asFUNCTION(FighterStatsCopyConstructor));
	getScriptEngine()->registerObjectProperty("FighterStats", "float maxHealth", asOFFSET(FighterStats, maxHealth));
	getScriptEngine()->registerObjectProperty("FighterStats", "string modelFilename", asOFFSET(FighterStats, modelFilename));
	getScriptEngine()->registerObjectProperty("FighterStats", "string weaponType", asOFFSET(FighterStats, weaponType));
	getScriptEngine()->registerObjectProperty("FighterStats", "WeaponStats weaponStats", asOFFSET(FighterStats, weaponStats));

	getScriptEngine()->registerObjectMethod("FighterStats", "void setWeaponStats(WeaponStats &in)", asMETHOD(FighterStats, setWeaponStats), asCALL_THISCALL);

	registered = true;
}
*/



namespace AngelScriptWrapperFunctions
{
	Fighter* FighterFactoryFunction()
	{
		return new Fighter();
	}
}

using namespace AngelScriptWrapperFunctions;


void registerFighter()
{
	static bool registered = false;
	if(registered)
		return;

	registerEntity();
	registerAttack();

	getScriptEngine()->registerObjectType("Fighter", sizeof(Fighter), asOBJ_REF | asOBJ_NOCOUNT );
	getScriptEngine()->registerFactoryFunction("Fighter", "Fighter@ f()", asFUNCTION(FighterFactoryFunction));

	getScriptEngine()->registerListType<Fighter*>("FighterList", "Fighter@");
	getScriptEngine()->registerFunction("FighterList getFighters()", asFUNCTION(getFighters), asCALL_CDECL);

	getScriptEngine()->registerObjectMethod("Fighter", "Fighter@ getClosestEnemy(FighterList = getFighters())", asMETHOD(Fighter, getClosestEnemy), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Fighter", "void useBestAttackOn(Fighter@)", asMETHOD(Fighter, useBestAttackOn), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Fighter", "Attack@ getCurrentAttack()", asMETHOD(Fighter, getCurrentAttack), asCALL_THISCALL);

	getScriptEngine()->registerObjectMethod("Fighter", "bool findAnimation()", asMETHOD(GameObject, findAnimation), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Fighter", "void playAnimation(string &in)", asMETHOD(GameObject, playAnimation), asCALL_THISCALL);


	getScriptEngine()->registerObjectMethod("Fighter", "void changeState(string &in)", asMETHOD(Fighter, changeState), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Fighter", "string getStateName()", asMETHOD(Fighter, getCurrentStateName), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("Fighter", "void returnToPreviousState()", asMETHOD(Fighter, returnToPreviousState), asCALL_THISCALL);


	registered = true;
}



