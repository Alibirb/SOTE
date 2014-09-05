#ifndef DANGERZONE_H
#define DANGERZONE_H

#include "Attachment.h"

#include "Damage.h"



/// An object representing a zone of danger (e.g. fire)
class DangerZone : public Attachment
{
protected:
	Damages _damages;
	float _radius;
	std::string _team;	/// Allows the effect to be masked by team.

public:
	DangerZone(osg::Group* parentNode);
	DangerZone(GameObjectData* dataObj, osg::Group* parentNode);
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
