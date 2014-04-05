#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

/*
#include <osgWidget/Input>

namespace osgWidget {
	class WindowManager;
}

using namespace osgWidget;
*/

#include "CEGUIStuff.h"
#include <CEGUI/WindowRendererSets/Core/MultiLineEditbox.h>

/// Text editor widget.
class TextEditor
{
private:
	bool _active;
	CEGUI::MultiLineEditbox* _editbox;
	CEGUI::Window* _window;

public:
	TextEditor(int width, int height);
	virtual ~TextEditor();

	bool isActive();
	void setActive(bool active);

};

#endif // TEXTEDITOR_H
