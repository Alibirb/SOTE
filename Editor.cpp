#ifdef _WIN32
	#include <windows.h>	/// Windows does some crazy shit if you don't include this file.
#endif

#include <GL/glew.h>

#include "Editor.h"



#include <osgWidget/Box>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Input>

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
	std::cout << "Creating Editor" << std::endl;

	//osgViewer::Viewer* viewer = getViewer();
	//_wm = new osgWidget::WindowManager(viewer, 1280.0f, 1024.0f, MASK_2D, osgWidget::WindowManager::WM_PICK_DEBUG);

	//osgWidget::Box* box = new osgWidget::Box("vbox", osgWidget::Box::VERTICAL);

	//osgWidget::Input* inputBox = new osgWidget::Input();
/*	osgWidget::Label* inputBox = new osgWidget::Label();
	inputBox->setFont("fonts/arial.ttf");
	inputBox->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
	inputBox->setFontSize(15);
	//inputBox->setYOffset(inputBox->calculateBestYOffset("y"));
	inputBox->setSize(500.0f, inputBox->getText()->getCharacterHeight() * 5.0);
	inputBox->setCanFill(true);
	inputBox->setLabel("Wait a second, wait a second, why am I on trial?\nYou only gave me inches when I need a million miles.");*/

	//getViewer()->getCamera()->getGraphicsContext()->makeCurrent();

	//glewInit();

	//if(glewInit() != GLEW_OK)
	//	std::cout << "GLEW failed" << std::endl;


	_geode = new osg::Geode;
	_geode->setName("CEGUI System");
	_geode->setDataVariance( osg::Object::DYNAMIC );

    {
        osg::StateSet* ss = _geode->getOrCreateStateSet();
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }

	_cd = new CEGUIDrawable();
	_geode->addDrawable(_cd.get());

	//CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	//CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

	_rootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	//_rootWindow = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow", "root");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_rootWindow);

	std::cout << "Editor created" << std::endl;

}

Editor::~Editor()
{
	_mode = EditorMode::Play;
}

/*
osgWidget::WindowManager* Editor::getWindowManager() {
	return _wm;
}*/

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
	TextEditor* textEditor = new TextEditor(600.0, 400.0);
}




Editor* getEditor() {
	static Editor* editor = new Editor();
	return editor;
}
