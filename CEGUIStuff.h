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

// See also https://github.com/xarray/osgRecipes/tree/master/cookbook/chapter9/ch09_04 for very similar implementation



#ifndef CEGUI_STUFF_H
#define CEGUI_STUFF_H

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/CoordinateSystemNode>
#include <osgGA/GUIEventAdapter>

#include <CEGUI/System.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>
//#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/ScriptModule.h>
#include <CEGUI/FontManager.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/Exceptions.h>
#include <CEGUI/GUIContext.h>

#include <CEGUI/CEGUI.h>

#include <iostream>

/// Drawable for the CEGUI system.
class CEGUIDrawable : public osg::Drawable
{
public:

    CEGUIDrawable();

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    CEGUIDrawable(const CEGUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):
        Drawable(drawable,copyop) {}

    META_Object(osg,CEGUIDrawable);

    void loadSchemeFromString(const std::string& scheme);
    void loadFontFromString(const std::string& font);
    void loadLayoutFromString(const std::string& layout);

    void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:

    virtual ~CEGUIDrawable();

    mutable unsigned int _activeContextID;
    mutable bool _initialized = false;

};


/// CEGUI calls glewInit() when setting up the system. GlewInit() has to be called from the graphics thread (not the main thread), so this wrapper is needed.
class CEGUIInitOperation : public osg::GraphicsOperation
{
public:
	CEGUIInitOperation():
		osg::GraphicsOperation("CEGUI Setup", false)
		{}

	virtual void operator() (osg::GraphicsContext* gc);
};

struct CEGUIEventCallback : public osgGA::GUIEventHandler
{
    CEGUIEventCallback(){}

    CEGUI::Key::Scan osgToCEGUIKey(int osgKey);

    bool isPrintableCEGUIKey(int ceguiKey);

    /** do customized Event code. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv);
};




#endif

