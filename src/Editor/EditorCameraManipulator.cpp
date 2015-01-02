#include "EditorCameraManipulator.h"

#include "Editor.h"

EditorCameraManipulator::EditorCameraManipulator()
{
	_distance = 10;
	_azimuth = 0.0f;
	_zenith = pi/4;
	_zenithMin = 0.001;
	_zenithMax = 3*pi/4;

	calculateRotation();

	/*osg::ref_ptr<osgGA::GUIEventAdapter> ea = new osgGA::GUIEventAdapter();
	osgViewer::Viewer* aa = getViewer();
	centerMousePointer(*ea, *aa);*/

	setCenter(osg::Vec3(0,0,0));

}

EditorCameraManipulator::~EditorCameraManipulator()
{
	//dtor
}


void EditorCameraManipulator::updateProjectionMatrix()
{
}

void EditorCameraManipulator::zoom(float deltaZoom)
{
#ifdef USE_LINEAR_SCROLLING
	_distance -= _deltaZoom;
#else
	_distance -= _distance * deltaZoom;
#endif//USE_LINEAR_SCROLLING

	if (_distance < 0)
		_distance = 0;
}

void EditorCameraManipulator::calculateRotation()
{
	_rotation =  osg::Quat(_zenith, osg::Vec3(1,0,0)) * osg::Quat(_azimuth, osg::Vec3(0,0,1));
}

osg::Vec3 EditorCameraManipulator::screenToWorldTranslation(osg::Vec3 point, double dx, double dy, double dz)
{

	osg::Matrix rotation_matrix;
	rotation_matrix.makeRotate(_rotation);

	/// Note that these methods return vectors for a y-up coordinate system, and we use z-up. That's why we get upVector from getFrontVector() and lookVector from getUpVector().
	osg::Vec3 lookVector = getUpVector(rotation_matrix);
	osg::Vec3 sideVector = -getSideVector(rotation_matrix);
	osg::Vec3 upVector = -getFrontVector(rotation_matrix);

	lookVector.normalize();
	sideVector.normalize();
	upVector.normalize();

	osg::Vec3 dv = upVector * dy + sideVector * dx + lookVector * dz;

	/// Scale the transformation according to the current projection. (note that this current implementation will fail if we don't use a perspective projection)
	double fovy, aspectRatio, zNear, zFar;
	getViewer()->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	double cameraToProjectionPlaneDistance = (windowHeight / 2) / tand(fovy/2);

	double cameraToPoint;
	osg::Vec3 cameraWorldPosition = getMatrix().getTrans();
	cameraToPoint = -getDistance(cameraWorldPosition, point);

	dv = dv * (cameraToPoint / cameraToProjectionPlaneDistance);

	return dv;
}


bool EditorCameraManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	/// Delegate handling to a method for the event type.

	switch( ea.getEventType() )
	{
	case GUIEventAdapter::FRAME:
		return handleFrame( ea, aa );

	case GUIEventAdapter::RESIZE:
		return handleResize( ea, aa );

	default:
		break;
	}


	if(!getEditor()->sceneHasFocus())
		return false;

	if(!_active)
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

bool EditorCameraManipulator::handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	float dx = ea.getX() - _lastMouseX;
	float dy = ea.getY() - _lastMouseY;

	/// If the difference is less than one pixel, ignore it.
	if(dx < 1 && dx > -1)
		dx = 0;
	if(dy < 1 && dy > -1)
		dy = 0;

	if( dx == 0.f && dy == 0.f )
		return false;	/// if there's no movement, return now.

	/// Rotate view when right mouse button is held
	if(ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		_azimuth -= dx * .01;
		_zenith -= dy * .01;
		if ( _zenith < _zenithMin) _zenith = _zenithMin;
		else if ( _zenith > _zenithMax) _zenith = _zenithMax;
		if ( _azimuth >= 2 * pi ) _azimuth -= 2 * pi;
		else if ( _azimuth < 0 ) _azimuth += 2 * pi;

		calculateRotation();
	}

	/// Pan when you hold the middle mouse button.
	if(ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		translate(-screenToWorldTranslation(_center, dx, dy));
	}


	/// Update the last mouse position for next time.
	_lastMouseX = ea.getX();
	_lastMouseY = ea.getY();

	return false;
}

bool EditorCameraManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	_lastMouseX = ea.getX();
	_lastMouseY = ea.getY();

	return false;
}


bool EditorCameraManipulator::handleMouseScroll( const GUIEventAdapter& ea, GUIActionAdapter& aa )
{
	if (ea.getScrollingMotion() == GUIEventAdapter::SCROLL_UP )
		zoom(_zoomFactor);
	else if (ea.getScrollingMotion() == GUIEventAdapter::SCROLL_DOWN)
		zoom(-_zoomFactor);
	return true;
}

bool EditorCameraManipulator::handleKeyDown(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	if(ea.getKey() == 'f')
		home(ea, aa);
}

bool EditorCameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	std::ostringstream stream;
	stream << "Camera center: " << _center.x() << ", " << _center.y() << ", " << _center.z() << "\n";
	stream << "Camera distance: " << _distance << "\n";


	double fovy, aspectRatio, zNear, zFar;
	getViewer()->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	stream << "fovy: " << fovy << "\n";
	stream << "aspectRatio: " << aspectRatio << "\n";
	stream << "zNear: " << zNear << "\n";
	stream << "zFar: " << zFar << "\n";

	osg::Matrix rotation_matrix;
	rotation_matrix.makeRotate(_rotation);
	osg::Vec3 upVector = -getUpVector(rotation_matrix);
	osg::Vec3 sideVector = getSideVector(rotation_matrix);
	osg::Vec3 frontVector = getFrontVector(rotation_matrix);

	stream << "up vector: " << upVector.x() << ", " << upVector.y() << ", " << upVector.z() << "\n";
	stream << "side vector: " << sideVector.x() << ", " << sideVector.y() << ", " << sideVector.z() << "\n";
	stream << "front vector: " << frontVector.x() << ", " << frontVector.y() << ", " << frontVector.z() << "\n";
	getDebugDisplayer()->addText(stream);
	return false;
}


