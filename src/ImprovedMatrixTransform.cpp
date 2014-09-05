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
	return getMatrix().getTrans();
}
void ImprovedMatrixTransform::setAttitude(osg::Quat attitude)
{
	osg::Matrix newMat = getMatrix();
	newMat.setRotate(attitude);
	setMatrix(newMat);
}
osg::Quat ImprovedMatrixTransform::getAttitude()
{
	return getMatrix().getRotate();
}
