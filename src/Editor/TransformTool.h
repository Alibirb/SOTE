#ifndef TRANSFORMTOOL_H
#define TRANSFORMTOOL_H


#include "Editor/Tool.h"

#include "ImprovedMatrixTransform.h"


/// Tool for basic transform operations.
class TransformTool : public Tool
{
protected:
	enum Mode {
		NO_MODE = 0,
		TRANSLATE,
		ROTATE,
		SCALE
	} _mode;

	enum ConstraintMode {
		NOT_CONSTRAINED = 0,
		CONSTRAIN_TO_X_AXIS,
		CONSTRAIN_TO_Y_AXIS,
		CONSTRAIN_TO_Z_AXIS
	} _constraintMode;

	double _lastMouseX, _lastMouseY;
	double _startingMouseX, _startingMouseY;

	osg::Vec3 _transformCenter;
	osg::Vec3 _transformCenterOnScreen;	/// point on the screen cooresponding to the center of the transform

	osg::Matrixd _transform;

	ImprovedMatrixTransform* _transformCenterNode;	/// Centers the transform on a point
	ImprovedMatrixTransform* _transformNode;		/// Parented to _transformCenterNode, contains the actual transform
	ImprovedMatrixTransform* _fakeOrigin;			/// Parented to _transformNode. The inverse of _transformCenterNode.

public:
	TransformTool();
	virtual ~TransformTool();

	virtual void onToolSelected();
	virtual void onToolDeselected();

	virtual void beginTransform();
	virtual void cancelTransform();
	virtual void applyTransform();

protected:
	virtual void onMouseMove(Event& event);
	virtual void onMouseButtonPush(Event& event);
	virtual void onKeyDown(Event& event);

};

#endif // TRANSFORMTOOL_H
