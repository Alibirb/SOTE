

#include "BaseCameraManipulator.h"


bool BaseCameraManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	// Delegate handling to a method for the event type.

	switch( ea.getEventType() )
	{
	case GUIEventAdapter::FRAME:
		return handleFrame( ea, aa );

	case GUIEventAdapter::RESIZE:
		return handleResize( ea, aa );

	default:
		break;
	}

	if( ea.getHandled() )	// The below events should only be handled by one event handler, so if the event's already been handled by something else, bail out.
		return false;

	switch( ea.getEventType() )
	{
	case GUIEventAdapter::MOVE:
		return handleMouseMove( ea, aa );

	case GUIEventAdapter::DRAG:
		return handleMouseDrag( ea, aa );

	case GUIEventAdapter::PUSH:
		return handleMousePush( ea, aa );

	case GUIEventAdapter::RELEASE:
		return handleMouseRelease( ea, aa );

	case GUIEventAdapter::SCROLL:
		return handleMouseScroll( ea, aa );

	case GUIEventAdapter::KEYDOWN:
		return handleKeyDown( ea, aa );

	case GUIEventAdapter::KEYUP:
		return handleKeyUp( ea, aa );

	default:
		return false;
	}
}



