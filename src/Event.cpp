#include "Event.h"

Event::Event()
{
	//ctor
}

Event::Event(const osgGA::GUIEventAdapter& osgEvent)
{
	/// This is easy, because we use the same enums as OSG uses (with some naming changes)
	setEventType((EventType) osgEvent.getEventType());
	setKey(osgEvent.getKey());
	setButton(osgEvent.getButton());

	_osgEvent = &osgEvent;
}

Event::~Event()
{
	//dtor
}
