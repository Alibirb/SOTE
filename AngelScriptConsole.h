#ifndef ANGELSCRIPTCONSOLE_H
#define ANGELSCRIPTCONSOLE_H

#include <osgWidget/Input>

#include "globals.h"

#include "AngelScriptEngine.h"
#include "input.h"

namespace osgWidget {
	class WindowManager;
}

using namespace osgWidget;

/// An interactive AngelScript console, for testing purposes.
class AngelScriptConsole : public osgWidget::Input
{
protected:
	bool active;

public:
	AngelScriptConsole();
	virtual ~AngelScriptConsole();

	bool keyDown(int key, int mask, const osgWidget::WindowManager* wm);


	bool isActive() {
		return active;
	}
	void setActive(bool active);

	bool mouseEnter(double, double, const WindowManager*);
	bool mouseLeave(double, double, const WindowManager*);

protected:
	void enterCommand(std::string code);
private:
};



#endif // ANGELSCRIPTCONSOLE_H
