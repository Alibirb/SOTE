#include "Fighter.h"
#include "Weapon.h"
#include "AngelScriptEngine.h"
#include "GameObjectData.h"

#include "TemporaryText.h"

#include "tinyxml/tinyxml2.h"

FighterStats::FighterStats()
{
}

float FighterStats::getResistance(std::string& damType)
{
	return resistances[damType];
}
void FighterStats::setResistance(std::string& type, float value)
{
	resistances[type] = value;
}



Fighter::Fighter(std::string name, osg::Vec3 position, std::string team) : Entity(name, position)
{
	this->_team = team;
	loadStats("media/Entities/" + name + ".as");
	equipWeapon(new Weapon(_stats.weaponStats));
}
Fighter::Fighter(XMLElement* xmlElement) : Entity()
{
	_equippedWeapon = NULL;
	load(xmlElement);
}
Fighter::Fighter(GameObjectData* dataObj) : Entity()
{
	_equippedWeapon = NULL;
	load(dataObj);
}



Fighter::~Fighter()
{
	delete _equippedWeapon;
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
	this->loadModel(_stats.modelFilename);
}


void Fighter::load(XMLElement* xmlElement)
{
	if(xmlElement->Attribute("source"))		/// Load from external source first, then apply changes.
		load(xmlElement->Attribute("source"));

	float x, y, z;
	xmlElement->QueryFloatAttribute("x", &x);
	xmlElement->QueryFloatAttribute("y", &y);
	xmlElement->QueryFloatAttribute("z", &z);

	initialPosition = osg::Vec3(x, y, z);
	setPosition(initialPosition);


	if(xmlElement->Attribute("maxHealth"))
		xmlElement->QueryFloatAttribute("maxHealth", &this->_stats.maxHealth);

	XMLElement* currentElement = xmlElement->FirstChildElement();
	for( ; currentElement; currentElement = currentElement->NextSiblingElement())
	{
		std::string elementType = currentElement->Value();
		if(elementType == "geometry")
			loadModel(currentElement->Attribute("source"));
		else if(elementType == "resistance")
		{
			std::string type = currentElement->Attribute("type");
			float value;
			currentElement->QueryFloatAttribute("value", &value);
			setResistance(type, value);
		}
		else if(elementType == "weapon")
		{
			if(getWeapon())
				markForRemoval(getWeapon(), "Item");	/// If we already have a weapon, we must be trying to override it. Delete the old one.
			equipWeapon(new Weapon(currentElement));
		}

	}
}

void Fighter::load(std::string xmlFilename)
{

	FILE *file = fopen(xmlFilename.c_str(), "rb");
	if(!file)
	{
		xmlFilename = "media/Entities/" + xmlFilename;
		file = fopen(xmlFilename.c_str(), "rb");
		if(!file)
			logError("Failed to open file " + xmlFilename);
	}


	XMLDocument doc(xmlFilename.c_str());
	if (doc.LoadFile(file)  != tinyxml2::XML_NO_ERROR)
	{
		logError("Failed to load file " + xmlFilename);
		logError(doc.GetErrorStr1());
	}


	XMLHandle docHandle(&doc);
	XMLElement* rootElement = docHandle.FirstChildElement().ToElement();

	load(rootElement);

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

float Fighter::getResistance(std::string type)
{
	return _stats.resistances[type];
}

void Fighter::setResistance(std::string type, float value)
{
	_stats.setResistance(type, value);
}

bool Fighter::isHurtByTeam(std::string otherTeam)
{
	return (_team.compare(otherTeam) != 0);
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
		dataObj->addChild(getWeapon()->save());
	dataObj->addData("maxHealth", _stats.maxHealth);
	for(auto kv : _stats.resistances)
	{
		GameObjectData* resistanceData = new GameObjectData("resistance");
		resistanceData->addData("type", kv.first);
		resistanceData->addData("value", kv.second);
		dataObj->addChild(resistanceData);
	}

}

void Fighter::load(GameObjectData* dataObj)
{
	loadGameObjectVariables(dataObj);
	loadEntityVariables(dataObj);
	loadFighterData(dataObj);
}
void Fighter::loadFighterData(GameObjectData* dataObj)
{
	/*dataObj->addData("team", _team);
	if(getWeapon())
		dataObj->addChild(getWeapon()->save());
	dataObj->addData("maxHealth", _stats.maxHealth);
	for(auto kv : _stats.resistances)
	{
		GameObjectData* resistanceData = new GameObjectData("resistance");
		resistanceData->addData("type", kv.first);
		resistanceData->addData("value", kv.second);
		dataObj->addChild(resistanceData);
	}*/

	_team = dataObj->getString("team");

	for(auto child: dataObj->getChildren())
	{
		if(child->getType() == "Weapon")
			equipWeapon(new Weapon(child));
		//else if(child->getType() = "")
	}

}


void addDamageIndicator(Fighter* entityHurt, float damageDealt, std::string& damageType)
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
