#include "TwoDimensionalCameraManipulator.h"
#include "Player.h"

void TwoDimensionalCameraManipulator::centerPlayerInView()
{
	setCenter(getActivePlayer()->getCameraTarget());
}

/*
void setByMatrix(const osg::Matrixd&)
{
}
void setByInverseMatrix(const osg::Matrixd&);
osg::Matrixd getMatrix() const;
osg::Matrixd getInverseMatrix() const;
*/
