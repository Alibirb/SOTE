#ifndef BASECAMERAMANIPULATOR_H
#define BASECAMERAMANIPULATOR_H


#include "osgGA/CameraManipulator"
#include "globals.h"

using namespace osgGA;
using namespace osg;

class BaseCameraManipulator : public osgGA::CameraManipulator
{
private:
	float currentZoom;
	float scrollAmount = .25f;

	osg::Vec3d _center;	// The point the camera is centered on
	osg::Quat _rotation;	// the rotation around the center
	double _distance;	// The distance from the center

	bool _verticalAxisFixed;	// Whether to fix the up vector.

public:


	void setCenter(osg::Vec3 newCenter)
	{
		_center = newCenter;
	}
	void translate(float dx, float dy, float dz = 0.0)
	{
		this->setCenter(_center+osg::Vec3(dx, dy, dz));
	}


	void setByMatrix(const osg::Matrixd& matrix)
	{
		_center = osg::Vec3d( 0., 0., -_distance ) * matrix;
		_rotation = matrix.getRotate();
	}
	void setByInverseMatrix(const osg::Matrixd& matrix) {
		setByMatrix( osg::Matrixd::inverse( matrix ) );
	}
	osg::Matrixd getMatrix() const {
		return osg::Matrixd::translate( 0, 0, _distance) *
				osg::Matrixd::rotate(_rotation) *
				osg::Matrixd::translate(_center);
	}
	osg::Matrixd getInverseMatrix() const {
		return osg::Matrixd::translate( -_center ) *
				osg::Matrixd::rotate( _rotation.inverse() ) *
				osg::Matrixd::translate( 0.0, 0.0, -_distance );
	}

	/** The method corrects the rotation to make impression of fixed up direction.
	 *  Technically said, it makes the roll component of the rotation equal to zero.
	 *
	 *  Up vector is given by CoordinateFrame and it is +z by default.
	 *  It can be changed by osgGA::CameraManipulator::setCoordinateFrameCallback().
	 *
	 *  Eye parameter is user position, rotation is the rotation to be fixed, and
	 *  disallowFlipOver, when set on true, avoids pitch rotation component to grow
	 *  over +/- 90 degrees. If this happens and disallowFlipOver is true,
	 *  manipulator is rotated by 180 degrees. More precisely, roll rotation component is changed by 180 degrees,
	 *  making pitch once again between -90..+90 degrees limits.*/
	void fixVerticalAxis( Vec3d& eye, Quat& rotation, bool disallowFlipOver )
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



	void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
	{
		_center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
		_rotation = rotation;

		// fix current rotation
		if( getVerticalAxisFixed() )
			fixVerticalAxis( _center, _rotation, true );
	}


	void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
	{
		eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
		rotation = _rotation;
	}


	void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
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


	void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
	{
		center = _center;
		eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
		up = _rotation * osg::Vec3d( 0., 1., 0. );
	}

	/// Fixes the "UP" vector to make camera control more user friendly
	void setVerticalAxisFixed( bool value ) {
		_verticalAxisFixed = value;
	}
	bool getVerticalAxisFixed() {
		return _verticalAxisFixed;
	}




	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void zoom(float amount) = 0;

	virtual void updateProjectionMatrix() = 0;	/// called to update the projection matrix, eg when the window size changes.

	virtual void home(const GUIEventAdapter& ea, GUIActionAdapter& aa)
	{
		setTransformation( _homeEye, _homeCenter, _homeUp );
		aa.requestRedraw();
	}


protected:
	virtual bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		return false;
	}
	virtual bool handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		// Update the global variables windowWidth and windowHeight.
		osgViewer::ViewerBase::Windows windows;
		viewer.getWindows(windows);
		windowWidth = ea.getWindowWidth();
		windowHeight = ea.getWindowHeight();
		updateProjectionMatrix();	// must change projection matrix.
		getDebugDisplayer()->updateProjection();	// The text displayer also needs to update itself.
		return false;
	}
	virtual bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		return false;
	}
	virtual bool handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		return false;
	}
	virtual bool handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		return false;
	}
	virtual bool handleMouseScroll( const GUIEventAdapter& ea, GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		if (ea.getKey() == 'r')
			this->home(ea, us);
		return false;
	}
	virtual bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
	{
		return false;
	}
private:

};

#endif // BASECAMERAMANIPULATOR_H
