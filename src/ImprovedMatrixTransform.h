#ifndef IMPROVEDMATRIXTRANSFORM_H
#define IMPROVEDMATRIXTRANSFORM_H

#include <osg/MatrixTransform>


class ImprovedMatrixTransform : public osg::MatrixTransform
{
public:
	ImprovedMatrixTransform();
	virtual ~ImprovedMatrixTransform();

	void setPosition(osg::Vec3 position);
	osg::Vec3 getPosition();
	void setAttitude(osg::Quat attitude);
	osg::Quat getAttitude();
	void setScale(osg::Vec3 scale);
	osg::Vec3 getScale();

	void makeIdentity();

protected:
private:
};

#endif // IMPROVEDMATRIXTRANSFORM_H
