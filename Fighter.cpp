#include "Fighter.h"
#include "Weapon.h"
#include "AngelScriptEngine.h"

#include "TemporaryText.h"

FighterStats::FighterStats()
{
}

float FighterStats::getResistance(DamageType& damType)
{
	return resistances[damType];
}
void FighterStats::setResistance(DamageType& type, float value)
{
	resistances[type] = value;
}



Fighter::Fighter(std::string name, osg::Vec3 position) : Entity(name, position)
{
	loadStats("media/Entities/" + name + ".as");
	//loadStats("media/Enemies/Human.as");
	equipWeapon(new Weapon(_stats.weaponStats));
	transformNode->addChild(equipedWeapon->getTransformNode());
}

Fighter::~Fighter()
{
	delete equipedWeapon;	// Note that this may not be the final desired behavior.
}

void Fighter::equipWeapon(Weapon *theWeapon)
{
	equipedWeapon = theWeapon;
}

void Fighter::aimWeapon(Entity *theOneWhoMustDie)
{
	equipedWeapon->aimAt(theOneWhoMustDie->getPosition());
}

Weapon* Fighter::getWeapon()
{
	return equipedWeapon;
}

void Fighter::setStats(FighterStats& newStats)
{
	this->_stats = newStats;
}

void Fighter::loadStats(std::string scriptFilename)
{
	registerFighterStats();
	getScriptEngine()->runFile(scriptFilename, "FighterStats loadStats()");
	setStats( *((FighterStats*) getScriptEngine()->getReturnObject()));
	this->health = _stats.maxHealth;
	this->setSpriteImage(_stats.imageFilename);
}

void Fighter::takeDamages(Damages dams)
{
	if(this->state == dead)
		return;
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

float Fighter::getResistance(DamageType& type)
{
	return _stats.resistances[type];
}


void addDamageIndicator(Fighter* entityHurt, float damageDealt, DamageType& damageType)
{
	std::ostringstream stream;
	stream << "-" << damageDealt;
	new TemporaryText(stream.str(), entityHurt->getWorldPosition(), 2.5);
}


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
		self->imageFilename = other.imageFilename;
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
	getScriptEngine()->registerObjectProperty("FighterStats", "string imageFilename", asOFFSET(FighterStats, imageFilename));
	getScriptEngine()->registerObjectProperty("FighterStats", "string weaponType", asOFFSET(FighterStats, weaponType));
	getScriptEngine()->registerObjectProperty("FighterStats", "WeaponStats weaponStats", asOFFSET(FighterStats, weaponStats));

	getScriptEngine()->registerObjectMethod("FighterStats", "void setResistance(DamageType &in, float value)", asMETHODPR(FighterStats, setResistance, (DamageType&, float), void), asCALL_THISCALL);
	getScriptEngine()->registerObjectMethod("FighterStats", "void setWeaponStats(WeaponStats &in)", asMETHOD(FighterStats, setWeaponStats), asCALL_THISCALL);

	registered = true;
}
