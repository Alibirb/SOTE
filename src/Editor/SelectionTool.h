#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "Tool.h"

/// Tool to select objects in the scene
class SelectionTool : public Tool
{
public:
	SelectionTool();
	virtual ~SelectionTool();

protected:

	virtual void onMouseButtonPush(Event& event);
	virtual void onMouseButtonRelease(Event& event);

};

#endif // SELECTIONTOOL_H
