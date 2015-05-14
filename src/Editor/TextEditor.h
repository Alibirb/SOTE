#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H


#include "CEGUIStuff.h"
#include <CEGUI/WindowRendererSets/Core/MultiLineEditbox.h>


#include "Editor/FileDialog.h"


/// Text editor widget.
class TextEditor
{
private:
	bool _active;
	CEGUI::MultiLineEditbox* _editbox;
	CEGUI::Window* _window;

	unsigned int _saveMenuItemID;
	unsigned int _loadFileMenuItemID;
	unsigned int _applyMenuItemID;
	unsigned int _loadSceneMenuItemID;

	unsigned int _itemIDsUsed;

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
	void loadFiles(FileDialog::FileList files);

};

#endif // TEXTEDITOR_H
