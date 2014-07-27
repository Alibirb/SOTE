#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H


#include "CEGUIStuff.h"
#include <CEGUI/WindowRendererSets/Core/MultiLineEditbox.h>

/// Text editor widget.
class TextEditor
{
private:
	bool _active;
	CEGUI::MultiLineEditbox* _editbox;
	CEGUI::Window* _window;

	uint _saveMenuItemID;
	uint _loadFileMenuItemID;
	uint _applyMenuItemID;
	uint _loadSceneMenuItemID;

	uint _itemIDsUsed;

	std::string _currentFilename;

	std::string _text;

public:
	TextEditor(const CEGUI::UVector2& position, const CEGUI::USize& size);
	virtual ~TextEditor();

	bool isActive();
	void setActive(bool active);

	bool onMenuItemClicked(const CEGUI::EventArgs& e);

	std::string getCurrentFilename();

	void saveFile(std::string filename);
	void saveFile();
	void loadFile(std::string filename);
	void loadFile();

};

#endif // TEXTEDITOR_H
