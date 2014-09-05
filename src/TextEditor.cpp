#include "TextEditor.h"

#include "input.h"


#include "globals.h"

#include "Editor.h"

#include "Level.h"


#include <iostream>
#include <fstream>

using namespace CEGUI;



TextEditor::TextEditor(const UVector2& position, const USize& size) : _itemIDsUsed(0)
{
	_window = static_cast<Window*>(WindowManager::getSingleton().createWindow("EditorLook/FrameWindow", "TextEditorWindow"));
	_window->setPosition(position);
	_window->setSize(size);
	_window->setText("ScumOfTheEarth Welcome/Instructions");


	float menubarHeight = 32;
	CEGUI::Menubar* menubar = static_cast<CEGUI::Menubar*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/Menubar", "TextEditorWindow/Menubar") );
	menubar->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	menubar->setSize(CEGUI::USize(UDim(1, 0), UDim(0, menubarHeight)) );
	_window->addChild(menubar);

	CEGUI::MenuItem* dropdownMenuItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "TextEditorWindow/Menubar/File") );
	dropdownMenuItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	dropdownMenuItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	dropdownMenuItem->setText("File");
	menubar->addChild(dropdownMenuItem);

	CEGUI::PopupMenu* dropdownMenu = static_cast<CEGUI::PopupMenu*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/PopupMenu", "TextEditorWindow/Menubar/File/PopupMenu") );
	dropdownMenu->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	dropdownMenu->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	dropdownMenuItem->addChild(dropdownMenu);


	CEGUI::MenuItem* loadFileItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "TextEditorWindow/Menubar/File/LoadFile") );
	loadFileItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	loadFileItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	loadFileItem->setText("Load scene from file");
	dropdownMenu->addChild(loadFileItem);
	loadFileItem->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&TextEditor::onMenuItemClicked, this));
	_loadFileMenuItemID = _itemIDsUsed;
	loadFileItem->setID(_itemIDsUsed);
	++_itemIDsUsed;

	CEGUI::MenuItem* saveItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "TextEditorWindow/Menubar/File/Save") );
	saveItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	saveItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	saveItem->setText("Save file");
	dropdownMenu->addChild(saveItem);
	saveItem->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&TextEditor::onMenuItemClicked, this));
	_saveMenuItemID = _itemIDsUsed;
	saveItem->setID(_itemIDsUsed);
	++_itemIDsUsed;

	CEGUI::MenuItem* loadSceneItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "TextEditorWindow/Menubar/File/LoadScene") );
	loadSceneItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	loadSceneItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	loadSceneItem->setText("Load scene into buffer");
	dropdownMenu->addChild(loadSceneItem);
	loadSceneItem->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&TextEditor::onMenuItemClicked, this));
	_loadSceneMenuItemID = _itemIDsUsed;
	loadSceneItem->setID(_itemIDsUsed);
	++_itemIDsUsed;

	CEGUI::MenuItem* applyItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "TextEditorWindow/Menubar/File/Apply") );
	applyItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	applyItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	applyItem->setText("Apply buffer to scene");
	dropdownMenu->addChild(applyItem);
	applyItem->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&TextEditor::onMenuItemClicked, this));
	_applyMenuItemID = _itemIDsUsed;
	applyItem->setID(_itemIDsUsed);
	++_itemIDsUsed;


	_editbox = static_cast<MultiLineEditbox*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MultiLineCodeEditbox", "TextEditorWindow/Editbox"));
	_editbox->setText("Controls: (Most only work when in \"Play\" mode)\n\nWASD: movement\nSpace: jump\n1: Previous character\n3: Next character\nE: Interact with object\nMouse: Adjust camera\nMouse wheel: Zoom camera in and out\nLeft mouse button (hold): Drag the mouse to teleport to another place in the scene\nF1: Toggle between \"Edit\" and \"Play\" modes\nF2: Export scene to file to reload\nF3: Toggle on-screen stats\nP: Toggle physics debug drawing (highly inefficient and slow)\nEscape: Exit SOTE");
	_editbox->setPosition( UVector2( UDim( 0.0f, 0.0f ), UDim( 0, (menubarHeight) ) ));
	_editbox->setSize(CEGUI::USize(CEGUI::UDim(1.0, 0.0), CEGUI::UDim(1.0, -menubarHeight)));
	_editbox->setReadOnly(false);
	_editbox->setWordWrapping(true);
	_window->addChild(_editbox);

	getEditor()->addWindow(_window);

	_currentFilename = "";
}

TextEditor::~TextEditor()
{
}

bool TextEditor::isActive() {
	return _active;
}
void TextEditor::setActive(bool active) {
	_active = active;
}

bool TextEditor::onMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);

	if(we.window->getID() == _saveMenuItemID)
	{
		saveFile();
		return true;
	}
	else if(we.window->getID() == _loadFileMenuItemID)
	{
		loadFile();
		return true;
	}
	else if(we.window->getID() == _applyMenuItemID)
	{
		getCurrentLevel()->loadFromString(_editbox->getText().c_str());
		return true;
	}
	else if(we.window->getID() == _loadSceneMenuItemID)
	{
		_editbox->setText(getCurrentLevel()->saveAsString());
		return true;
	}

	return false;
}

std::string TextEditor::getCurrentFilename()
{
	return _currentFilename;
}

void TextEditor::saveFile(std::string filename)
{
	std::ofstream theFile;
	theFile.open(filename);

	theFile << _editbox->getText().c_str();
	theFile.close();
}
void TextEditor::saveFile()
{
	saveFile(_currentFilename);
}

void TextEditor::loadFile(std::string filename)
{
	_currentFilename = filename;
	std::ifstream theFile;
	theFile.open(_currentFilename, std::ios::in);

	std::stringstream buffer;

	buffer << theFile.rdbuf();

	_editbox->setText(buffer.str());
	_window->setText(filename);

	theFile.close();

}
void TextEditor::loadFile()
{
	if(_currentFilename == "")
		_currentFilename = getCurrentLevel()->getFilename();

	loadFile(_currentFilename);
}

