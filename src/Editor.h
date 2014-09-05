#ifndef EDITOR_H
#define EDITOR_H


#include <CEGUI/WindowManager.h>
#include <osg/Geode>

class CEGUIDrawable;


/// The game editor
class Editor
{
public:
	enum EditorMode {
		Play,
		Edit
	};

private:
	osg::ref_ptr<osg::Geode> _geode;
    osg::ref_ptr<CEGUIDrawable> _cd;
	EditorMode _mode;
	CEGUI::Window* _rootWindow;

public:
	Editor();
	virtual ~Editor();

	void setMode(EditorMode mode);

	void addWindow(CEGUI::Window* window);

	void setupDefaultWindows();
};

Editor* getEditor();

#endif // EDITOR_H
