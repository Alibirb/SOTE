#ifndef TOOLBAR_H
#define TOOLBAR_H


#include "CEGUIStuff.h"

#include "Tool.h"

#include "unordered_map"


/// Toolbar for the Editor.
class Toolbar
{
protected:
	CEGUI::Window* _window;
	CEGUI::PopupMenu* _dropdownMenu;

	std::unordered_map<uint,Tool*> _tools;	/// Map of available tools, using the CEGUI window ID as a key.

	uint _itemIDsUsed = 0;

public:
	Toolbar();
	~Toolbar();

	void addEntryForTool(Tool* tool);	/// Adds an entry for the given tool to the "Tools" menu

	bool onToolsMenuItemClicked(const CEGUI::EventArgs& e);

protected:

};

#endif // TOOLBAR_H
