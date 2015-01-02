#include "ImprovedMatrixTransform.h"

ImprovedMatrixTransform::ImprovedMatrixTransform()
{
	//ctor
}

ImprovedMatrixTransform::~ImprovedMatrixTransform()
{
	//dtor
}

void ImprovedMatrixTransform::setPosition(osg::Vec3 position)
{
	osg::Matrix newMat = getMatrix();
	newMat.setTrans(position);
	setMatrix(newMat);
}
osg::Vec3 ImprovedMatrixTransform::getPosition()
{
	osg::Vec3d translation;
	osg::Quat rotation;
	osg::Vec3d scale;
	osg::Quat scaleOrientation;

	getMatrix().decompose(translation, rotation, scale, scaleOrientation);

	return translation;
}

void ImprovedMatrixTransform::setAttitude(osg::Quat attitude)
{
	osg::Quat currentRotation = getAttitude();
	_matrix.preMultRotate(currentRotation.inverse());
	_matrix.preMultRotate(attitude);
}
osg::Quat ImprovedMatrixTransform::getAttitude()
{
	osg::Vec3d translation;
	osg::Quat rotation;
	osg::Vec3d scale;
	osg::Quat scaleOrientation;

	getMatrix().decompose(translation, rotation, scale, scaleOrientation);

	return rotation;
}

void ImprovedMatrixTransform::setScale(osg::Vec3 scale)
{
	/// Note: In the Transform header file, it says we need to use a StateSet to recalculate normals if we're using a scale. Should check on that.

	osg::Vec3d translation;
	osg::Quat rotation;
	osg::Vec3d currentScale;
	osg::Quat scaleOrientation;

	getMatrix().decompose(translation, rotation, currentScale, scaleOrientation);

	makeIdentity();
	_matrix.postMultScale(scale);
	setPosition(translation);
	setAttitude(rotation);

}
osg::Vec3 ImprovedMatrixTransform::getScale()
{
	osg::Vec3d translation;
	osg::Quat rotation;
	osg::Vec3d scale;
	osg::Quat scaleOrientation;

	getMatrix().decompose(translation, rotation, scale, scaleOrientation);

	return scale;
}

void ImprovedMatrixTransform::makeIdentity()
{
	_matrix.makeIdentity();
}

