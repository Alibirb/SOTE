#include "Tool.h"

Tool::Tool() : _name("Default Tool")
{
}

Tool::~Tool()
{
}


void Tool::onEvent(Event& event)
{
	switch(event.getEventType())
	{
	case Event::MOUSE_PUSH:
		onMouseButtonPush(event);
		break;
	case Event::MOUSE_RELEASE:
		onMouseButtonRelease(event);
		break;
	case Event::MOUSE_DRAG:
		onMouseDrag(event);
		break;
	case Event::MOUSE_MOVE:
		onMouseMove(event);
		break;
	case Event::KEY_DOWN:
		onKeyDown(event);
		break;
	case Event::KEY_UP:
		onKeyUp(event);
		break;
	}

}

