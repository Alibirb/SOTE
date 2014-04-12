#ifndef DANGERZONE_H
#define DANGERZONE_H

#include "GameObject.h"

#include "Damage.h"



/// An object representing a zone of danger (e.g. fire)
class DangerZone : public GameObject
{
protected:
	Damages _damages;
	float _radius;
	std::string _team;	/// Allows the effect to be masked by team.

public:
	DangerZone();
	DangerZone(GameObjectData* dataObj);
	virtual ~DangerZone();

	Damages getDamages();
	std::string getTeam();

	virtual GameObjectData* save();
	virtual void load(GameObjectData* dataObj);

protected:
	void saveDangerZoneVariables(GameObjectData* dataObj);
	void loadDangerZoneVariables(GameObjectData* dataObj);
};

#endif // DANGERZONE_H
