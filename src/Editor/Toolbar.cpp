#include "Toolbar.h"

#include "Editor.h"


using namespace CEGUI;

Toolbar::Toolbar()
{
	float height = 32;

	_window = static_cast<CEGUI::Menubar*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/Menubar", "Toolbar") );
	_window->setPosition(CEGUI::UVector2(UDim(0,0), UDim(0,0)));
	_window->setSize(CEGUI::USize(UDim(1,0), UDim(0, height)));


	CEGUI::MenuItem* dropdownMenuItem = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "Toolbar/Tools") );
	dropdownMenuItem->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	dropdownMenuItem->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	dropdownMenuItem->setText("Tools");
	_window->addChild(dropdownMenuItem);

	_dropdownMenu = static_cast<CEGUI::PopupMenu*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/PopupMenu", "Toolbar/Tools/PopupMenu") );
	_dropdownMenu->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	_dropdownMenu->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	dropdownMenuItem->addChild(_dropdownMenu);

	getEditor()->addWindow(_window);

}

Toolbar::~Toolbar()
{

}


void Toolbar::addEntryForTool(Tool* tool)
{
	uint itemID;

	CEGUI::MenuItem* item = static_cast<CEGUI::MenuItem*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/MenuItem", "Toolbar/File/LoadFile") );
	item->setPosition( CEGUI::UVector2(UDim(0, 0), UDim(0, 0)) );
	item->setSize(CEGUI::USize(UDim(1, 0), UDim(1, 0)) );
	item->setText(tool->getName());
	_dropdownMenu->addChild(item);
	item->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&Toolbar::onToolsMenuItemClicked, this));
	itemID = _itemIDsUsed;
	item->setID(_itemIDsUsed);
	++_itemIDsUsed;

	_tools[itemID] = tool;

}

bool Toolbar::onToolsMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);

	if(_tools.count(we.window->getID()) > 0)
	{
		getEditor()->setCurrentTool(_tools[we.window->getID()]);
		return true;
	}

	return false;
}

