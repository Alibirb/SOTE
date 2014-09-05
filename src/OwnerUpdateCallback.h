#ifndef OWNERUPDATECALLBACK_H
#define OWNERUPDATECALLBACK_H

#include "osg/NodeCallback"

double getDeltaTime();

/// osg NodeCallback that calls the owner's onUpdate function
template<class T>
class OwnerUpdateCallback : public osg::NodeCallback
{
public:
	T* _owner;

	OwnerUpdateCallback(T* owner)
	{
		this->_owner = owner;
	}

	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		_owner->onUpdate(getDeltaTime());
	}
};

#endif // OWNERUPDATECALLBACK_H
