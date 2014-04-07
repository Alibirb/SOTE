
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This application is open source and may be redistributed and/or modified
 * freely and without restriction, both in commercial and non commercial applications,
 * as long as this copyright notice is maintained.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

// Initially taken from http://trac.openscenegraph.org/projects/osg/browser/OpenSceneGraph/trunk/examples/osgcegui/osgcegui.cpp?rev=11931



#include "CEGUIStuff.h"

#include "globals.h"

CEGUIDrawable::CEGUIDrawable()
{
    setSupportsDisplayList(false);
    setDataVariance( osg::Object::DYNAMIC );
	getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    setEventCallback(new CEGUIEventCallback());

    //new CEGUI::System( CEGUI::OpenGLRenderer::create() );
    CEGUI::OpenGLRenderer::bootstrapSystem();


    // initialise the required dirs for the DefaultResourceProvider
	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	//CEGUI::ResourceProvider* rp = CEGUI::System::getSingleton().getResourceProvider();
	rp->setResourceGroupDirectory("schemes", "./datafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets", "./datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "./datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "./datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "./datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "./datafiles/lua_scripts/");

	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");


    _activeContextID = 0;
}

CEGUIDrawable::~CEGUIDrawable()
{
    // delete CEGUI??
}

void CEGUIDrawable::loadSchemeFromString(const std::string& scheme)
{
    try
    {
        CEGUI::SchemeManager::getSingleton().createFromString(scheme.c_str());
    }
    catch (CEGUI::Exception e)
    {
        std::cout<<"CEGUIDrawable::loadScheme Error: "<<e.getMessage()<<std::endl;
    }
}

void CEGUIDrawable::loadFontFromString(const std::string& font)
{
    try
    {
        CEGUI::FontManager::getSingleton().createFromString(font.c_str());
    }
    catch (CEGUI::Exception e)
    {
        std::cout<<"CEGUIDrawable::loadFont Error: "<<e.getMessage()<<std::endl;
    }
}

void CEGUIDrawable::loadLayoutFromString(const std::string& layout)
{
    try
    {
        CEGUI::Window* myRoot = CEGUI::WindowManager::getSingleton().loadLayoutFromString(layout.c_str());
        //CEGUI::System::getSingleton().setGUISheet(myRoot);
        CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);

    }
    catch (CEGUI::Exception e)
    {
        std::cout<<"CEGUIDrawable::loadLayout error: "<<e.getMessage()<<std::endl;
    }

}

void CEGUIDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
/*
    osg::State& state = *renderInfo.getState();

    if (state.getContextID()!=_activeContextID) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    state.disableAllVertexArrays();

   // CEGUI::System::getSingleton().renderGUI();
    CEGUI::System::getSingleton().renderAllGUIContexts();


    glPopAttrib();

    state.checkGLErrors("CEGUIDrawable::drawImplementation");
*/
	osg::State* state = renderInfo.getState();
	if (state->getContextID()!=_activeContextID)
		return;
	state->disableAllVertexArrays();
	state->disableTexCoordPointer( 0 );

	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	CEGUI::OpenGLRenderer* renderer = static_cast<CEGUI::OpenGLRenderer*>(
	CEGUI::System::getSingleton().getRenderer() );
	osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();
	if ( renderer && viewport )
	{
		const CEGUI::Sizef& size = renderer->getDisplaySize();
		if ( size.d_width!=viewport->width() || size.d_height!=viewport->height() )
		{
			CEGUI::System::getSingleton().notifyDisplaySizeChanged(
			CEGUI::Sizef(viewport->width(), viewport->height()) );
		}
	}

	double currentTime = (state->getFrameStamp() ? state->getFrameStamp()->getSimulationTime() : 0.0);
	//  CEGUI::System::getSingleton().injectTimePulse( (currentTime - _lastSimulationTime)/1000.0 );
	CEGUI::System::getSingleton().injectTimePulse(getDeltaTime());
	CEGUI::System::getSingleton().renderAllGUIContexts();
      //  _lastSimulationTime = currentTime;

		//std::cout << "GUI rendered" << std::endl;

	glPopAttrib();
	glPopMatrix();
}


bool CEGUIEventCallback::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv)
{
	osgGA::EventVisitor* ev = dynamic_cast<osgGA::EventVisitor*>(nv);
	CEGUIDrawable* cd = dynamic_cast<CEGUIDrawable*>(obj);

	if (!ev || !cd) return false;

	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::DRAG):
		case(osgGA::GUIEventAdapter::MOVE):
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(ea.getX(), ea.getYmax() - ea.getY());
			return true;
		case(osgGA::GUIEventAdapter::PUSH):
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(ea.getX(), ea.getYmax() - ea.getY());

			if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MiddleButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton);

			return true;
		}
		case(osgGA::GUIEventAdapter::RELEASE):
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(ea.getX(), ea.getYmax() - ea.getY());

			if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::MiddleButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::RightButton);

			return true;
		}
		case(osgGA::GUIEventAdapter::DOUBLECLICK):
		{
			// do we need to do something special here to handle double click???  Will just assume button down for now.
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(ea.getX(), ea.getYmax() - ea.getY());

			if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MiddleButton);

			else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
			  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton);

			return true;
		}
		case(osgGA::GUIEventAdapter::KEYDOWN):
			//CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown( static_cast<CEGUI::uint>(ea.getKey()) );
			//CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown( static_cast<CEGUI::Key::Scan>(ea.getKey()) );
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown( osgToCEGUIKey(ea.getKey()) );
			//CEGUI::System::getSingleton().getDefaultGUIContext().injectChar( static_cast<CEGUI::utf32>( ea.getKey() ) );
			CEGUI::System::getSingleton().getDefaultGUIContext().injectChar( osgToCEGUIKey(ea.getKey()) );
			std::cout << "OSG: " << ea.getKey() << std::endl;
			//std::cout << "Fed to CEGUI: " << static_cast<CEGUI::Key::Scan>(ea.getKey()) << std::endl;
			std::cout << "Fed to CEGUI: " << osgToCEGUIKey(ea.getKey()) << std::endl;
			//std::cout << "Char fed to CEGUI: " << static_cast<CEGUI::utf32>( ea.getKey() ) << std::endl;
			std::cout << "Char fed to CEGUI: " << osgToCEGUIKey(ea.getKey()) << std::endl;
			//std::cout << ea.getKey() << std::endl;
			return true;
		case(osgGA::GUIEventAdapter::KEYUP):
			//CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp( static_cast<CEGUI::Key::Scan>(ea.getKey()) );
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp( osgToCEGUIKey(ea.getKey()) );
			return true;
		default:
			break;
	}

	return false;
}

 CEGUI::Key::Scan CEGUIEventCallback::osgToCEGUIKey(int osgKey)
{
	#define UNKNOWN_KEY_CASE(osgKeyName) case osgGA::GUIEventAdapter::osgKeyName: std::cout << "Key unregonized by CEGUI, osg keycode is " << osgKeyName << "." << std::endl return 0;
	#define KEY_CASE(osgKeyName, CEGUIKeyName) case osgGA::GUIEventAdapter::osgKeyName: return CEGUI::Key::CEGUIKeyName;
	switch(osgKey)
	{

	KEY_CASE(KEY_BackSpace, Backspace)
	KEY_CASE(KEY_Tab, Tab)
	//UNKNOWN_KEY_CASE(KEY_Linefeed)
	//KEY_CASE(KEY_Clear, Clear)
	KEY_CASE(KEY_Return, Return)
	KEY_CASE(KEY_Pause, Pause)
	KEY_CASE(KEY_Scroll_Lock, ScrollLock)
	KEY_CASE(KEY_Sys_Req, SysRq)
	KEY_CASE(KEY_Escape, Escape)
	KEY_CASE(KEY_Delete, Delete)

	KEY_CASE(KEY_Home, Home)
	KEY_CASE(KEY_Left, ArrowLeft)
	KEY_CASE(KEY_Up, ArrowUp)
	KEY_CASE(KEY_Right, ArrowRight)
	KEY_CASE(KEY_Down, ArrowDown)
	//KEY_CASE(KEY_Prior, Prev)	// duplicate of page up
	KEY_CASE(KEY_Page_Up, PageUp)
	//KEY_CASE(KEY_Next, Next)	// duplicate of page down
	KEY_CASE(KEY_Page_Down, PageDown)
	KEY_CASE(KEY_End, End)	// EOL
	//KEY_CASE(KEY_Begin, Begin)	// BOL

	//KEY_CASE(KEY_Select, Select)
	//KEY_CASE(KEY_Print, Print)
	//KEY_CASE(KEY_Execute, Execute)
	KEY_CASE(KEY_Insert, Insert)
	//KEY_CASE(KEY_Undo, Undo)
	//KEY_CASE(KEY_Redo, Redo)
	//KEY_CASE(KEY_Menu, Menu)
	//KEY_CASE(KEY_Find, Find)
	//KEY_CASE(KEY_Cancel, Cancel)
	//KEY_CASE(KEY_Help, Help)
	//KEY_CASE(KEY_Break, Break)
	//KEY_CASE(KEY_Mode_switch, Mode_switch)
	//KEY_CASE(KEY_Script_switch, Script_switch)	// alias for script switch
	KEY_CASE(KEY_Num_Lock, NumLock)

	KEY_CASE(KEY_KP_Space, Space)
	KEY_CASE(KEY_KP_Tab, Tab)
	KEY_CASE(KEY_KP_Enter, NumpadEnter)
	//KEY_CASE(KEY_KP_F1, NumpadF1)
	//KEY_CASE(KEY_KP_F2, NumpadF2)
	//KEY_CASE(KEY_KP_F3, NumpadF3)
	//KEY_CASE(KEY_KP_F4, NumpadF4)
	KEY_CASE(KEY_KP_Home, Home)
	KEY_CASE(KEY_KP_Left, ArrowLeft)
	KEY_CASE(KEY_KP_Up, ArrowUp)
	KEY_CASE(KEY_KP_Right, ArrowRight)
	KEY_CASE(KEY_KP_Down, ArrowDown)
	//KEY_CASE(KEY_KP_Prior, Prior)
	KEY_CASE(KEY_KP_Page_Up, PageUp)
	//KEY_CASE(KEY_KP_Next, Next)
	KEY_CASE(KEY_KP_Page_Down, PageDown)
	KEY_CASE(KEY_KP_End, End)
	//KEY_CASE(KEY_KP_Begin, Begin)
	KEY_CASE(KEY_KP_Insert, Insert)
	KEY_CASE(KEY_KP_Delete, Delete)
	KEY_CASE(KEY_KP_Equal, NumpadEquals)
	KEY_CASE(KEY_KP_Multiply, Multiply)
	KEY_CASE(KEY_KP_Add, Add)
	KEY_CASE(KEY_KP_Separator, NumpadComma)
	KEY_CASE(KEY_KP_Subtract, Subtract)
	KEY_CASE(KEY_KP_Decimal, Decimal)
	KEY_CASE(KEY_KP_Divide, Divide)

	KEY_CASE(KEY_KP_0, Numpad0)
	KEY_CASE(KEY_KP_1, Numpad1)
	KEY_CASE(KEY_KP_2, Numpad2)
	KEY_CASE(KEY_KP_3, Numpad3)
	KEY_CASE(KEY_KP_4, Numpad4)
	KEY_CASE(KEY_KP_5, Numpad5)
	KEY_CASE(KEY_KP_6, Numpad6)
	KEY_CASE(KEY_KP_7, Numpad7)
	KEY_CASE(KEY_KP_8, Numpad8)
	KEY_CASE(KEY_KP_9, Numpad9)

	KEY_CASE(KEY_F1, F1)
	KEY_CASE(KEY_F2, F2)
	KEY_CASE(KEY_F3, F3)
	KEY_CASE(KEY_F4, F4)
	KEY_CASE(KEY_F5, F5)
	KEY_CASE(KEY_F6, F6)
	KEY_CASE(KEY_F7, F7)
	KEY_CASE(KEY_F8, F8)
	KEY_CASE(KEY_F9, F9)
	KEY_CASE(KEY_F10, F10)
	KEY_CASE(KEY_F11, F11)
	KEY_CASE(KEY_F12, F12)
	KEY_CASE(KEY_F13, F13)
	KEY_CASE(KEY_F14, F14)
	KEY_CASE(KEY_F15, F15)


	KEY_CASE(KEY_Shift_L, LeftShift)
	KEY_CASE(KEY_Shift_R, RightShift)
	KEY_CASE(KEY_Control_L, LeftControl)
	KEY_CASE(KEY_Control_R, RightControl)
	KEY_CASE(KEY_Caps_Lock, Capital)	// ??? Not sure if Capital really means CapsLock
	//KEY_CASE(KEY_Shift_Lock

	//KEY_CASE(KEY_Meta_L
	//KEY_CASE(Key_Meta_R
	KEY_CASE(KEY_Alt_L, LeftAlt)
	KEY_CASE(KEY_Alt_R, RightAlt)
	KEY_CASE(KEY_Super_L, LeftWindows)
	KEY_CASE(KEY_Super_R, RightWindows)
	//KEY_CASE(KEY_Hyper_L,
	//KEY_CASE(KEY_Hyper_R,




	default:
		return (CEGUI::Key::Scan)osgKey;

	}
	#undef KEY_CASE

}

