#ifdef _WIN32
	#include <windows.h>	/// Windows does some crazy shit if you don't include this file.
#endif

#include <GL/glew.h>

#include "Editor/Editor.h"



#include "globals.h"

#include "input.h"

#include "Editor/EditorCameraManipulator.h"
#include "Editor/TextEditor.h"
#include "Editor/Toolbar.h"
#include "Editor/Tool.h"
#include "Editor/SelectionTool.h"
#include "Editor/TransformTool.h"
#include "Editor/ObjectOverlay.h"
#include "Editor/FileDialog.h"

#include "CEGUIStuff.h"

#include "GameObject.h"

#include "Level.h"

#include <CEGUI/DefaultResourceProvider.h>
#include <CEGUI/ImageManager.h>
#include <CEGUI/CEGUI.h>

#include <stdio.h>
#ifdef _WIN32
    #include <direct.h>
#else
    #include <unistd.h>
#endif


#include <fstream>

#include <osgwTools/Shapes.h>


const unsigned int MASK_2D = 0xF0000001;


Editor::Editor() : _currentTool(NULL), _toolbar(NULL)
{
	_ceguiGeode = new osg::Geode;
	_ceguiGeode->setName("CEGUI System");
	_ceguiGeode->setDataVariance(osg::Object::DYNAMIC);

	osg::StateSet* ss = _ceguiGeode->getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	_cd = new CEGUIDrawable();
	_ceguiGeode->addDrawable(_cd.get());

	_rootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_rootWindow);


	addTool(new SelectionTool());
	addTool(new TransformTool());

	_cameraManipulator = new EditorCameraManipulator();
}

Editor::~Editor()
{
	_mode = EditorMode::Play;
}


void Editor::setMode(EditorMode mode)
{
	switch(mode)
	{
	case EditorMode::Play:
		getMainEventHandler()->setInputMode(MainEventHandler::Standard);
		getViewer()->setCameraManipulator(getCurrentLevel()->getCameraManipulator(), false);
		removeFromSceneGraph(_ceguiGeode);	/// TODO: should use an osg::Switch node
		for(GameObject* object : _selectedObjects)
			object->getSelectionOverlay()->setActive(false);
		break;
	case EditorMode::Edit:
		getMainEventHandler()->setInputMode(MainEventHandler::Editor);
		getViewer()->setCameraManipulator(_cameraManipulator, false);
		addToSceneGraph(_ceguiGeode);
		for(GameObject* object : _selectedObjects)
			object->getSelectionOverlay()->setActive(true);
		break;
	}
	_mode = mode;
}

void Editor::addWindow(CEGUI::Window* window)
{
	_rootWindow->addChild(window);
}

void Editor::setupDefaultWindows()
{
	_toolbar = new Toolbar();

	for(Tool* tool : _tools)
		_toolbar->addEntryForTool(tool);

	TextEditor* textEditor = new TextEditor(CEGUI::UVector2(cegui_reldim(2.0/3), cegui_reldim(0)), CEGUI::USize(cegui_reldim(1.0/3), cegui_reldim(1)));

	//FileDialog* dialog = new FileDialog();
}


void Editor::onEvent(Event& event)
{
	switch (event.getKey())
	{
	case Event::KEY_F1:
		setMode(Editor::Play);
		return;
	}

	/// If the scene doesn't have focus, ignore the event (CEGUI will handle it instead)
	if(sceneHasFocus())
	{
		/// If we have a tool selected, send it the event.
		if(_currentTool)
			_currentTool->onEvent(event);
		else
		{
			if(event.getEventType() == Event::KEY_DOWN && _toolKeyBindings[event.getKey()])
			{
				setCurrentTool(_toolKeyBindings[event.getKey()]);
				_currentTool->onEvent(event);
			}

			if(event.getEventType() == Event::MOUSE_PUSH && _toolButtonBindings[event.getButton()])
			{
				setCurrentTool(_toolButtonBindings[event.getButton()]);
				_currentTool->onEvent(event);
			}

		}
	}
}

void Editor::addTool(Tool* tool)
{
	_tools.push_back(tool);
	for(unsigned i : tool->getActivationKeys())
		if(_toolKeyBindings[i] == NULL)
			_toolKeyBindings[i] = tool;
		//else
			// we've got a problem.
	for(unsigned i : tool->getActivationButtons())
		if(_toolButtonBindings[i] == NULL)
			_toolButtonBindings[i] = tool;
		//else
			// we've got a problem.

	if(_toolbar)
		_toolbar->addEntryForTool(tool);
}

bool Editor::sceneHasFocus()
{
	if(_rootWindow->getActiveChild() == _rootWindow || _rootWindow->getActiveChild() == NULL)
		return true;
	else
		return false;
}


std::list<GameObject*> Editor::getSelectedObjects()
{
	return _selectedObjects;
}
void Editor::setSelectedObjects(std::list<GameObject*> objects)
{
	/// Remove the overlays from any currently-selected objects
	for(GameObject* object : _selectedObjects)
		object->getSelectionOverlay()->setActive(false);

	_selectedObjects = objects;

	/// Add overlays to the new group of selected objects
	for(GameObject* object : _selectedObjects)
		object->getSelectionOverlay()->setActive(true);
}
void Editor::unselectObject(GameObject* object)
{
	_selectedObjects.remove(object);
}


Tool* Editor::getCurrentTool()
{
	return _currentTool;
}
void Editor::setCurrentTool(Tool* tool)
{
	if(_currentTool)
		_currentTool->onToolDeselected();

	_currentTool = tool;

	if(_currentTool)
		_currentTool->onToolSelected();
}



Editor* getEditor() {
	static Editor* editor = new Editor();
	return editor;
}

