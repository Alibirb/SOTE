#ifndef BASECAMERAMANIPULATOR_H
#define BASECAMERAMANIPULATOR_H


#include "osgGA/CameraManipulator"
#include "osg/Quat"
#include "globals.h"

using namespace osgGA;
using namespace osg;

class BaseCameraManipulator : public osgGA::CameraManipulator
{
protected:
	float currentZoom;
	float scrollAmount = .25f;

	osg::Vec3d _center;	// The point the camera is centered on
	osg::Quat _rotation;	// the rotation around the center
	double _distance;	// The distance from the center
	osg::Vec3d _up;		// the up axis

	bool _verticalAxisFixed;	// Whether to fix the up vector.

	double _azimuth = 0.0;	// heading
	double _zenith = pi/4;	// inclination angle

	float _windowCenterX;
	float _windowCenterY;

public:
	void setCenter(osg::Vec3 newCenter);
	void translate(float dx, float dy, float dz = 0.0);

	void setByMatrix(const osg::Matrixd& matrix);
	void setByInverseMatrix(const osg::Matrixd& matrix);
	osg::Matrixd getMatrix() const;
	osg::Matrixd getInverseMatrix() const;

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
	void fixVerticalAxis( Vec3d& eye, Quat& rotation, bool disallowFlipOver );

	void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );
	void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const;
	void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
	void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const;

	/// Fixes the "UP" vector to make camera control more user friendly
	void setVerticalAxisFixed( bool value );
	bool getVerticalAxisFixed();

	float getHeading()
	{
		return _azimuth;
	}
	float getZenith()
	{
		return _zenith;
	}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void zoom(float deltaZoom) = 0;

	virtual void updateProjectionMatrix() = 0;	/// called to update the projection matrix, eg when the window size changes.

	virtual void home(const GUIEventAdapter& ea, GUIActionAdapter& aa);

	/// Set the rotation based on azimuth (heading) and zenith (inclination)
	virtual void setRotation(double azimuth, double zenith);

	void centerMousePointer(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);


protected:
	virtual bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		return false;
	}
	virtual bool handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	virtual bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleMouseScroll( const GUIEventAdapter& ea, GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
	virtual bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		return false;
	}
private:

};

#endif // BASECAMERAMANIPULATOR_H
