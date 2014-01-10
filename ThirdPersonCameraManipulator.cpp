/*
 * ThirdPersonCameraManipulator.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: daniel
 */

#include "ThirdPersonCameraManipulator.h"
#include "TextDisplay.h"
#include <osgViewer/View>


ThirdPersonCameraManipulator::ThirdPersonCameraManipulator()
{
	_distance = 10;
	_minDistance = 1;
	_maxDistance = 500;
	_azimuth = 0.0f;
	_zenith = pi/4;
	_zenithMin = 0.001;
	_zenithMax = 3*pi/4;
	calculateRotation();

	osg::ref_ptr<osgGA::GUIEventAdapter> ea = new osgGA::GUIEventAdapter();
	centerMousePointer(*ea, viewer);

}

void ThirdPersonCameraManipulator::updateProjectionMatrix()
{
	//viewer.apply(new osgViewer::SingleWindow(0, 0, windowWidth, windowHeight, 0));
//	viewer.getDisplaySettings()->setScreenWidth(windowWidth);
//	viewer.getDisplaySettings()->setScreenHeight(windowHeight);
}

void ThirdPersonCameraManipulator::zoom(float deltaZoom)
{
#ifdef USE_LINEAR_SCROLLING
	_distance -= _deltaZoom;
#else
	_distance -= _distance * deltaZoom;
#endif//USE_LINEAR_SCROLLING

	if (_distance < _minDistance)
		_distance = _minDistance;
	else if (_distance > _maxDistance)
		_distance = _maxDistance;
}

bool ThirdPersonCameraManipulator::handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	float dx = ea.getX() - _windowCenterX;
	float dy = ea.getY() - _windowCenterY;

	// If the difference is less than one pixel, ignore it. This tolerance is essential in case we're unable to warp the cursor to the exact center(for example, if the center is a non-integer).
	if(dx < 1 && dx > -1)
		dx = 0;
	if(dy < 1 && dy > -1)
		dy = 0;

	if( dx == 0.f && dy == 0.f ) return false;	// if there's no movement, return now.

	centerMousePointer( ea, aa );

	_azimuth -= dx * .001;
	_zenith -= dy * .001;
	if ( _zenith < _zenithMin) _zenith = _zenithMin;
	else if ( _zenith > _zenithMax) _zenith = _zenithMax;
	if ( _azimuth >= 2 * pi ) _azimuth -= 2 * pi;
	else if ( _azimuth < 0 ) _azimuth += 2 * pi;

	calculateRotation();

	return true;
}

// triggered in same way as MouseMove, but only when a mouse button is held down.
bool ThirdPersonCameraManipulator::handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	getDebugDisplayer()->addText("Drag Detected\n");
	return false;
}

bool ThirdPersonCameraManipulator::handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	getDebugDisplayer()->addText("Mouse Button Push Detected: ");
	getDebugDisplayer()->addText(ea.getButton());

	// Backward and forward buttons are now implemented (on Linux, at least)
	switch(ea.getButton())
	{
#ifdef SUPPORTS_EXTRA_MOUSE_BUTTONS
	case osgGA::GUIEventAdapter::FORWARD_MOUSE_BUTTON :
		zoom(_zoomFactor);
		aa.requestRedraw();
		return true;
	case osgGA::GUIEventAdapter::BACKWARD_MOUSE_BUTTON :
		zoom(-_zoomFactor);
		aa.requestRedraw();
		return true;
#endif
	default:
		return false;
	}

	return false;
}
bool ThirdPersonCameraManipulator::handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	if (ea.getButton() == GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (view->computeIntersections(ea, intersections ))
		{
			getActivePlayer()->setPosition(intersections.begin()->getWorldIntersectPoint());
		}
	}
	centerMousePointer( ea, aa );
	return false;
}

bool ThirdPersonCameraManipulator::handleMouseScroll( const GUIEventAdapter& ea, GUIActionAdapter& aa )
{
	if (ea.getScrollingMotion() == GUIEventAdapter::SCROLL_UP )
		zoom(_zoomFactor);
	else if (ea.getScrollingMotion() == GUIEventAdapter::SCROLL_DOWN)
		zoom(-_zoomFactor);
	return true;
}

