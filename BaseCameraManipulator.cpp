
#include "BaseCameraManipulator.h"


void BaseCameraManipulator::setCenter(osg::Vec3 newCenter)
{
	_center = newCenter;
}
void BaseCameraManipulator::translate(float dx, float dy, float dz)
{
	this->setCenter(_center+osg::Vec3(dx, dy, dz));
}

void BaseCameraManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	_center = osg::Vec3d( 0., 0., -_distance ) * matrix;
	_rotation = matrix.getRotate();
}
void BaseCameraManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix( osg::Matrixd::inverse( matrix ) );
}

osg::Matrixd BaseCameraManipulator::getMatrix() const
{
	return osg::Matrixd::translate( 0, 0, _distance) *
		   osg::Matrixd::rotate(_rotation) *
		   osg::Matrixd::translate(_center);
}
osg::Matrixd BaseCameraManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate( -_center ) *
		   osg::Matrixd::rotate( _rotation.inverse() ) *
		   osg::Matrixd::translate( 0.0, 0.0, -_distance );
}

void BaseCameraManipulator::fixVerticalAxis( Vec3d& eye, Quat& rotation, bool disallowFlipOver )
{
	CoordinateFrame coordinateFrame = getCoordinateFrame( eye );
	Vec3d localUp = getUpVector( coordinateFrame );

	//fixVerticalAxis( rotation, localUp, disallowFlipOver );

	// camera direction vectors
	Vec3d cameraUp = rotation * Vec3d( 0.,1.,0. );
	Vec3d cameraRight = rotation * Vec3d( 1.,0.,0. );
	Vec3d cameraForward = rotation * Vec3d( 0.,0.,-1. );

	// computed directions
	Vec3d newCameraRight1 = cameraForward ^ localUp;
	Vec3d newCameraRight2 = cameraUp ^ localUp;
	Vec3d newCameraRight = (newCameraRight1.length2() > newCameraRight2.length2()) ?
						   newCameraRight1 : newCameraRight2;
	if( newCameraRight * cameraRight < 0. )
		newCameraRight = -newCameraRight;

	// vertical axis correction
	Quat rotationVerticalAxisCorrection;
	rotationVerticalAxisCorrection.makeRotate( cameraRight, newCameraRight );

	// rotate camera
	rotation *= rotationVerticalAxisCorrection;

	if( disallowFlipOver )
	{

		// make viewer's up vector to be always less than 90 degrees from "up" axis
		Vec3d newCameraUp = newCameraRight ^ cameraForward;
		if( newCameraUp * localUp < 0. )
			rotation = Quat( PI, Vec3d( 0.,0.,1. ) ) * rotation;

	}
}

void BaseCameraManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
	_center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
	_rotation = rotation;

	// fix current rotation
	if( getVerticalAxisFixed() )
		fixVerticalAxis( _center, _rotation, true );
}

void BaseCameraManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
	eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
	rotation = _rotation;
}

void BaseCameraManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
	Vec3d lv( center - eye );

	Vec3d f( lv );
	f.normalize();
	Vec3d s( f^up );
	s.normalize();
	Vec3d u( s^f );
	u.normalize();

	osg::Matrixd rotation_matrix( s[0], u[0], -f[0], 0.0f,
								  s[1], u[1], -f[1], 0.0f,
								  s[2], u[2], -f[2], 0.0f,
								  0.0f, 0.0f,  0.0f, 1.0f );

	_center = center;
	_distance = lv.length();
	_rotation = rotation_matrix.getRotate().inverse();

	// fix current rotation
	if( getVerticalAxisFixed() )
		fixVerticalAxis( _center, _rotation, true );
}

void BaseCameraManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
	center = _center;
	eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
	up = _rotation * osg::Vec3d( 0., 1., 0. );
}


void BaseCameraManipulator::setVerticalAxisFixed( bool value )
{
	_verticalAxisFixed = value;
}
bool BaseCameraManipulator::getVerticalAxisFixed()
{
	return _verticalAxisFixed;
}




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


void BaseCameraManipulator::home(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	setTransformation( _homeEye, _homeCenter, _homeUp );
	aa.requestRedraw();
}

/// Set the rotation based on azimuth (heading) and zenith (inclination)
void BaseCameraManipulator::setRotation(double azimuth, double zenith)
{
	_rotation =  osg::Quat(zenith, osg::Vec3(1,0,0)) * osg::Quat(azimuth, _up);
}

void BaseCameraManipulator::centerMousePointer(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	_windowCenterX = (ea.getXmin() + ea.getXmax()) / 2.0f;
	_windowCenterY = (ea.getYmin() + ea.getYmax()) / 2.0f;
	aa.requestWarpPointer( _windowCenterX, _windowCenterY);
}

bool BaseCameraManipulator::handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	// Update the global variables windowWidth and windowHeight.
	osgViewer::ViewerBase::Windows windows;
	getViewer()->getWindows(windows);
	windowWidth = ea.getWindowWidth();
	windowHeight = ea.getWindowHeight();
	updateProjectionMatrix();	// must change projection matrix.
	getDebugDisplayer()->updateProjection();	// The text displayer also needs to update itself.
	return false;
}
