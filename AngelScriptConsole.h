#ifndef ANGELSCRIPTCONSOLE_H
#define ANGELSCRIPTCONSOLE_H

#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Input>
#include <osgWidget/ViewerEventHandlers>

#include "globals.h"

#include "AngelScriptEngine.h"
#include "input.h"

using namespace osgWidget;

/// An interactive AngelScript console, for testing purposes.
class AngelScriptConsole : public osgWidget::Input
{
protected:
	osgWidget::Box* box;
	osgWidget::WindowManager* wm;
	bool active;

public:
	AngelScriptConsole();
	virtual ~AngelScriptConsole();

	bool keyDown(int key, int mask, const osgWidget::WindowManager* wm);


	bool isActive() {
		return active;
	}
	void setActive(bool active) {
		this->active = active;
		if(active)
		{
			getMainEventHandler()->setInputMode(MainEventHandler::InputMode::Console);
		}
		else
		{
			getMainEventHandler()->setInputMode(MainEventHandler::InputMode::Standard);
		}
	}

	bool mouseEnter(double, double, const WindowManager*)
	{
		setActive(true);
		return true;
	}
	bool mouseLeave(double, double, const WindowManager*)
	{
		setActive(false);
		return true;
	}

protected:
	void enterCommand(std::string code);
private:
};



#endif // ANGELSCRIPTCONSOLE_H
