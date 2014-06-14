#include "Attachment.h"

#include "Light.h"
#include "DangerZone.h"

#include "globals.h"


Attachment::Attachment(osg::Group* parentNode) : GameObject(parentNode)
{

}

Attachment* Attachment::create(GameObjectData* dataObj, osg::Group* parentNode)
{
	#define Attachment_create_elseif(objectType) else if(type == #objectType) return new objectType (dataObj, parentNode);
	std::string type = dataObj->getType();
	if(type == "Light")
		return new Light(dataObj, parentNode);
	Attachment_create_elseif(DangerZone)

	else
		logError("Unknown Attachment type '" + type + "' requested.");
}
