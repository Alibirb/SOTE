/// Initially taken from http://trac.openscenegraph.org/projects/osg/browser/OpenSceneGraph/trunk/examples/osgcegui/osgcegui.cpp?rev=11931
/// Initial copyright notice:
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



#include <GL/glew.h>

#include "CEGUIStuff.h"

#include "globals.h"

#include "Editor/Editor.h"

#include "Editor/MultiLineCodeEditbox.h"



using namespace CEGUI;


bool onMouseLeaves(const CEGUI::EventArgs &e)
{
	const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(e);
	mouseArgs.window->deactivate();

	return true;
}
bool onMouseEnters(const CEGUI::EventArgs &e)
{
	const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(e);
	mouseArgs.window->activate();

	return true;
}


void CEGUIInitOperation::operator() (osg::GraphicsContext* gc)
{
	CEGUI::OpenGLRenderer::bootstrapSystem();

	std::string guiResourceBase = "./media/GUI/";

	// initialise the required dirs for the DefaultResourceProvider
	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("schemes", guiResourceBase + "schemes/");
	rp->setResourceGroupDirectory("imagesets", guiResourceBase + "imagesets/");
	rp->setResourceGroupDirectory("fonts", guiResourceBase + "fonts/");
	rp->setResourceGroupDirectory("layouts", guiResourceBase + "layouts/");
	rp->setResourceGroupDirectory("looknfeels", guiResourceBase + "looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", guiResourceBase + "lua_scripts/");

	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	CEGUI::SchemeManager::getSingleton().createFromFile("EditorLook.scheme");

	CEGUI::WindowFactoryManager::getSingleton().addFactory<TplWindowFactory<MultiLineCodeEditbox> >();
	CEGUI::WindowRendererManager::getSingleton().addWindowRendererType<MultiLineCodeEditboxWindowRenderer>();
	WindowFactoryManager::getSingleton().addFalagardWindowMapping("EditorLook/MultiLineCodeEditbox",
                                                                  "MultiLineCodeEditbox",
                                                                  "EditorLook/MultiLineEditbox",
                                                                  "MultiLineCodeEditbox");

	/// Set up global events to change the input focus based on which window the cursor is over.
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(CEGUI::Window::EventNamespace + "/" + CEGUI::Window::EventMouseEntersSurface, CEGUI::Event::Subscriber(&onMouseEnters));
	CEGUI::GlobalEventSet::getSingleton().subscribeEvent(CEGUI::Window::EventNamespace + "/" + CEGUI::Window::EventMouseLeavesSurface, CEGUI::Event::Subscriber(&onMouseLeaves));

	getEditor()->setupDefaultWindows();
}

CEGUIDrawable::CEGUIDrawable()
{
	setSupportsDisplayList(false);
	setDataVariance( osg::Object::DYNAMIC );
	getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
	getOrCreateStateSet()->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);	/// To avoid CEGUI bug when using multitextured objects.
	getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");                     	/// Also for that bug
	getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

	setEventCallback(new CEGUIEventCallback());

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

	CEGUI::OpenGLRenderer* renderer = static_cast<CEGUI::OpenGLRenderer*>(CEGUI::System::getSingleton().getRenderer() );
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
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(osgToCEGUIKey(ea.getUnmodifiedKey()));


			if(isPrintableCEGUIKey(osgToCEGUIKey(ea.getKey())))	/// Only inject the character if it's meant to be printed. (Don't inject the arrow keys or that stuff)
			{
				uint32 unicodeChar;
				unicodeChar = ea.getKey();
				CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(unicodeChar);
			}

			if(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)
			{
				switch(ea.getUnmodifiedKey())
				{
				case 'c':
					std::cout << "Injecting copy request" << std::endl;
					CEGUI::System::getSingleton().getDefaultGUIContext().injectCopyRequest();
					break;
				case 'v':
					std::cout << "Injecting paste request" << std::endl;
					CEGUI::System::getSingleton().getDefaultGUIContext().injectPasteRequest();
					break;
				case 'x':
					std::cout << "Injecting cut request" << std::endl;
					CEGUI::System::getSingleton().getDefaultGUIContext().injectCutRequest();
					break;
				}
			}


			return true;
		case(osgGA::GUIEventAdapter::KEYUP):
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp( osgToCEGUIKey(ea.getUnmodifiedKey()) );
			return true;
		case osgGA::GUIEventAdapter::SCROLL:

			if(ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP)
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(1);
			else if(ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN)
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(-1);
			return true;
		default:
			break;
	}

	return false;
}

bool CEGUIEventCallback::isPrintableCEGUIKey(int ceguiKey)
{
	#define NOT_PRINTABLE(ceguiKeyName) case CEGUI::Key::ceguiKeyName:

	return true;

	switch(ceguiKey)
	{
		NOT_PRINTABLE(Unknown)
        NOT_PRINTABLE(Escape)
        NOT_PRINTABLE(LeftControl)
        NOT_PRINTABLE(LeftShift)
        NOT_PRINTABLE(LeftAlt)
        NOT_PRINTABLE(Capital)
        NOT_PRINTABLE(F1)
        NOT_PRINTABLE(F2)
        NOT_PRINTABLE(F3)
        NOT_PRINTABLE(F4)
        NOT_PRINTABLE(F5)
        NOT_PRINTABLE(F6)
        NOT_PRINTABLE(F7)
        NOT_PRINTABLE(F8)
        NOT_PRINTABLE(F9)
		NOT_PRINTABLE(F10)
        NOT_PRINTABLE(NumLock)
        NOT_PRINTABLE(ScrollLock)
      //  OEM_102         =0x56,    /* < > | on UK/Germany keyboards */
        NOT_PRINTABLE(F11)
        NOT_PRINTABLE(F12)
        NOT_PRINTABLE(F13)
        NOT_PRINTABLE(F14)
        NOT_PRINTABLE(F15)
       /* Kana            =0x70,    // (Japanese keyboard)
        ABNT_C1         =0x73,    // / ? on Portugese (Brazilian) keyboards
        Convert         =0x79,    // (Japanese keyboard)
        NoConvert       =0x7B,    // (Japanese keyboard)
        Yen             =0x7D,    // (Japanese keyboard)
        ABNT_C2         =0x7E,    // Numpad . on Portugese (Brazilian) keyboards
        PrevTrack       =0x90,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
        At              =0x91,    //                     (NEC PC98)
        Underline       =0x93,    //                     (NEC PC98)
        Kanji           =0x94,    // (Japanese keyboard)
        Stop            =0x95,    //                     (NEC PC98)
        AX              =0x96,    //                     (Japan AX)
        Unlabeled       =0x97,    //                        (J3100)
        */
        NOT_PRINTABLE(NextTrack)
        NOT_PRINTABLE(RightControl)
        NOT_PRINTABLE(Mute)
        NOT_PRINTABLE(Calculator)
        NOT_PRINTABLE(PlayPause)
        NOT_PRINTABLE(MediaStop)
        NOT_PRINTABLE(VolumeDown)
        NOT_PRINTABLE(VolumeUp)
        NOT_PRINTABLE(WebHome)
        NOT_PRINTABLE(SysRq)
        NOT_PRINTABLE(RightAlt)
        NOT_PRINTABLE(Pause)
        NOT_PRINTABLE(Home)
        NOT_PRINTABLE(ArrowUp)
        NOT_PRINTABLE(PageUp)
        NOT_PRINTABLE(ArrowLeft)
        NOT_PRINTABLE(ArrowRight)
        NOT_PRINTABLE(End)
        NOT_PRINTABLE(ArrowDown)
        NOT_PRINTABLE(PageDown)
        NOT_PRINTABLE(Insert)
        NOT_PRINTABLE(Delete)
        NOT_PRINTABLE(LeftWindows)
        NOT_PRINTABLE(RightWindows)
        NOT_PRINTABLE(AppMenu)
        NOT_PRINTABLE(Power)
        NOT_PRINTABLE(Sleep)
        NOT_PRINTABLE(Wake)
        NOT_PRINTABLE(WebSearch)
        NOT_PRINTABLE(WebFavorites)
        NOT_PRINTABLE(WebRefresh)
        NOT_PRINTABLE(WebStop)
        NOT_PRINTABLE(WebForward)
        NOT_PRINTABLE(WebBack)
        NOT_PRINTABLE(MyComputer)
        NOT_PRINTABLE(Mail)
        NOT_PRINTABLE(MediaSelect)

		return false;
	default:
		return true;
	}
}

 CEGUI::Key::Scan CEGUIEventCallback::osgToCEGUIKey(int osgKey)
{
	#define UNKNOWN_KEY_CASE(osgKeyName) case osgGA::GUIEventAdapter::osgKeyName: std::cout << "Key unrecognized by CEGUI, osg keycode is " << osgKeyName << "." << std::endl return 0;
	#define KEY_CASE(osgKeyName, CEGUIKeyName) case osgGA::GUIEventAdapter::osgKeyName: return CEGUI::Key::CEGUIKeyName;
	switch(osgKey)
	{

	//KEY_Exclaim         = 0x21,
	KEY_CASE(KEY_Quotedbl, Grave)
	//KEY_Hash,
	//KEY_Dollar          = 0x24,
	//KEY_CASE(KEY_Ampersand, Ampersand)
	KEY_CASE(KEY_Quote, Apostrophe)
	//KEY_Leftparen       = 0x28,
	//KEY_Rightparen      = 0x29,
	//KEY_CASE(KEY_Asterisk, Asterisk)
	//KEY_CASE(KEY_Plus, Plus)
	KEY_CASE(KEY_Comma, Comma)
	KEY_CASE(KEY_Minus, Minus)
	KEY_CASE(KEY_Period, Period)
	KEY_CASE(KEY_Slash, Slash)
	KEY_CASE(KEY_Colon, Colon)
	KEY_CASE(KEY_Semicolon, Semicolon)
	//KEY_Less,
	KEY_CASE(KEY_Equals, Equals)
	//KEY_Greater         = 0x3E,
	//KEY_Question        = 0x3F,
	KEY_CASE(KEY_At, At)
	KEY_CASE(KEY_Leftbracket, LeftBracket)
	KEY_CASE(KEY_Backslash, Backslash)
	KEY_CASE(KEY_Rightbracket, RightBracket)
	//KEY_Caret           = 0x5E,
	//KEY_Underscore      = 0x5F,
	//KEY_Backquote       = 0x60,


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

	KEY_CASE(KEY_A, A)
	KEY_CASE(KEY_B, B)
	KEY_CASE(KEY_C, C)
	KEY_CASE(KEY_D, D)
	KEY_CASE(KEY_E, E)
	KEY_CASE(KEY_F, F)
	KEY_CASE(KEY_G, G)
	KEY_CASE(KEY_H, H)
	KEY_CASE(KEY_I, I)
	KEY_CASE(KEY_J, J)
	KEY_CASE(KEY_K, K)
	KEY_CASE(KEY_L, L)
	KEY_CASE(KEY_M, M)
	KEY_CASE(KEY_N, N)
	KEY_CASE(KEY_O, O)
	KEY_CASE(KEY_P, P)
	KEY_CASE(KEY_Q, Q)
	KEY_CASE(KEY_R, R)
	KEY_CASE(KEY_S, S)
	KEY_CASE(KEY_T, T)
	KEY_CASE(KEY_U, U)
	KEY_CASE(KEY_V, V)
	KEY_CASE(KEY_W, W)
	KEY_CASE(KEY_X, X)
	KEY_CASE(KEY_Y, Y)
	KEY_CASE(KEY_Z, Z)

	KEY_CASE(KEY_0, Zero)
	KEY_CASE(KEY_1, One)
	KEY_CASE(KEY_2, Two)
	KEY_CASE(KEY_3, Three)
	KEY_CASE(KEY_4, Four)
	KEY_CASE(KEY_5, Five)
	KEY_CASE(KEY_6, Six)
	KEY_CASE(KEY_7, Seven)
	KEY_CASE(KEY_8, Eight)
	KEY_CASE(KEY_9, Nine)


	default:
		return CEGUI::Key::Unknown;
	}
	#undef KEY_CASE

}

