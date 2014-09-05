#ifdef _WIN32
	#include <windows.h>	/// Windows does some crazy shit if you don't include this file.
#endif

#include <GL/glew.h>

#include "Editor.h"



#include "globals.h"

#include "input.h"
#include "BaseCameraManipulator.h"

#include "TextEditor.h"

#include "CEGUIStuff.h"

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


const unsigned int MASK_2D = 0xF0000001;

using namespace CEGUI;


Editor::Editor()
{
	_geode = new osg::Geode;
	_geode->setName("CEGUI System");
	_geode->setDataVariance( osg::Object::DYNAMIC );

    {
        osg::StateSet* ss = _geode->getOrCreateStateSet();
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }

	_cd = new CEGUIDrawable();
	_geode->addDrawable(_cd.get());

	_rootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_rootWindow);
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
		((BaseCameraManipulator*)getViewer()->getCameraManipulator())->setActive(true);
		removeFromSceneGraph(_geode);	/// TODO: should use an osg::Switch node
		break;
	case EditorMode::Edit:
		getMainEventHandler()->setInputMode(MainEventHandler::Editor);
		((BaseCameraManipulator*)getViewer()->getCameraManipulator())->setActive(false);
		addToSceneGraph(_geode);
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
	TextEditor* textEditor = new TextEditor(UVector2(cegui_reldim(2.0/3), cegui_reldim(0)), USize(cegui_reldim(1.0/3), cegui_reldim(1)));
}




Editor* getEditor() {
	static Editor* editor = new Editor();
	return editor;
}

