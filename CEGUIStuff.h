
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

    /** do customized Event code. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv);
};


/*
int main( int argc, char **argv )
{

    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);


    // construct the viewer.
    osgViewer::Viewer viewer;


    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<CEGUIDrawable> cd = new CEGUIDrawable();
    geode->addDrawable(cd.get());

    std::string scheme;
    while(arguments.read("--scheme",scheme))
    {
        cd->loadScheme(scheme);
    }

    std::string font;
    while(arguments.read("--font",font))
    {
        cd->loadFont(font);
    }

    std::string layout;
    while(arguments.read("--layout",layout))
    {
        cd->loadLayout(layout);
    }

    osg::Timer_t start_tick = osg::Timer::instance()->tick();

    // read the scene from the list of file specified command line args.
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);

    // if no model has been successfully loaded report failure.
    if (!loadedModel)
    {
        std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
        return 1;
    }

    osg::ref_ptr<osg::Group> group = new osg::Group;
    group->addChild(loadedModel.get());

    group->addChild(geode.get());


    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
    }

    osg::Timer_t end_tick = osg::Timer::instance()->tick();

    std::cout << "Time to load = "<<osg::Timer::instance()->delta_s(start_tick,end_tick)<<std::endl;


    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    // pass the loaded scene graph to the viewer.
    viewer.setSceneData(group.get());

    // run the viewer
    return viewer.run();
}
*/


#endif

