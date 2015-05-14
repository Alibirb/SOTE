#ifndef EDITOR_H
#define EDITOR_H


#include <CEGUI/WindowManager.h>
#include <osg/Geode>

class CEGUIDrawable;


#include "Event.h"

class GameObject;
class Tool;
class Toolbar;

class ObjectOverlay;
class EditorCameraManipulator;
class BaseCameraManipulator;


/// The game editor
class Editor
{
public:
	enum EditorMode {
		Play,
		Edit
	};

private:
	osg::ref_ptr<osg::Geode> _ceguiGeode;
    osg::ref_ptr<CEGUIDrawable> _cd;
	EditorMode _mode;
	CEGUI::Window* _rootWindow;

	std::list<GameObject*> _selectedObjects;
	Tool* _currentTool;

	std::list<Tool*> _tools;

	Toolbar* _toolbar;

	osg::ref_ptr<EditorCameraManipulator> _cameraManipulator;

	std::map<unsigned, Tool*> _toolKeyBindings;
	std::map<unsigned, Tool*> _toolButtonBindings;

public:
	Editor();
	~Editor();

	void setMode(EditorMode mode);
	EditorMode getMode() {return _mode;}

	void addWindow(CEGUI::Window* window);

	void setupDefaultWindows();

	void onEvent(::Event& event);

	void addTool(Tool* tool);

	/// Returns whether the scene view has focus, as opposed to some GUI element
	bool sceneHasFocus();


	std::list<GameObject*> getSelectedObjects();
	void setSelectedObjects(std::list<GameObject*> objects);
	void unselectObject(GameObject* object);

	Tool* getCurrentTool();
	void setCurrentTool(Tool* tool);

	std::list<Tool*> getTools()	{return _tools;}

	EditorCameraManipulator* getCameraManipulator() {return _cameraManipulator;}

	CEGUIDrawable* getCeguiDrawable()	{return _cd;}
};

Editor* getEditor();

#endif // EDITOR_H
