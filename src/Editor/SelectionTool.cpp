#include "SelectionTool.h"

#include "globals.h"


#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>

#include "osgNodeUserData.h"

#include "GameObject.h"

#include "Editor/Editor.h"

#include "input.h"



SelectionTool::SelectionTool()
{
	_name = "Selection tool";
	_description = "Select objects with left mouse button. Hold Shift to select multiple objects.";

	_activationButtons.push_back(Event::LEFT_MOUSE_BUTTON);
}

SelectionTool::~SelectionTool()
{
	//dtor
}


void SelectionTool::onMouseButtonPush(Event& event)
{
	switch(event.getButton())
	{
	case Event::LEFT_MOUSE_BUTTON:
	{
		/// Select an object
		const osgGA::GUIEventAdapter* ea = event.getOsgEvent();
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ray = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION, ea->getXnormalized(), ea->getYnormalized());
		osgUtil::IntersectionVisitor visitor(ray);

		getViewer()->getCamera()->accept(visitor);

		if (ray->containsIntersections())
		{
			osgUtil::LineSegmentIntersector::Intersection intersection = ray->getFirstIntersection();

			osg::NodePath& nodes = intersection.nodePath;

			for (int i = nodes.size() - 1; i >= 0; i--)
			{
				osg::ref_ptr<osgNodeUserData> userData = dynamic_cast<osgNodeUserData*>(nodes[i]->getUserData());
				if (userData && userData->owner)
				{
					std::list<GameObject*> objectList;
					if(getMainEventHandler()->keyState[Event::KEY_Shift_L] || getMainEventHandler()->keyState[Event::KEY_Shift_R])
					{
						/// When Shift is held, we modify the current object selection. If the new object is already selected, we unselect it. If not, we select it in addition to whatever's already selected.
						objectList = getEditor()->getSelectedObjects();

						bool alreadySelected = false;
						for(GameObject* obj : objectList)
							if(obj == userData->owner)
							{
								alreadySelected = true;
								break;
							}

						if(alreadySelected)
							objectList.remove(userData->owner);
						else
							objectList.push_back(userData->owner);
					}
					else
						objectList.push_back(userData->owner);

					getEditor()->setSelectedObjects(objectList);
					break;
				}
			}
		}
		break;
	}
	default:
		break;
	}
}

void SelectionTool::onMouseButtonRelease(Event& event)
{
	switch(event.getButton())
	{
	case Event::LEFT_MOUSE_BUTTON:
		getEditor()->setCurrentTool(NULL);
		break;
	}
}

