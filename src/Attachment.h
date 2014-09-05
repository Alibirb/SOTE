#ifndef ATTACHMENT_H
#define ATTACHMENT_H

#include "GameObject.h"

/// Class for things to attach to GameObjects
/// TODO: explain this better.
class Attachment : public GameObject
{
protected:
	Attachment(osg::Group* parentNode);

public:
	virtual ~Attachment(){}

	static Attachment* create(GameObjectData* dataObj, osg::Group* parentNode);

};

#endif // ATTACHMENT_H
