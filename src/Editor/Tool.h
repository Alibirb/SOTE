#ifndef TOOL_H
#define TOOL_H


#include "Event.h"


/// Base class for editor tools
class Tool
{
protected:
	std::string _name;	/// Name of the tool, shows up in the tool's menu entry.
	std::string _description;	/// Short description of what the tool does and how to use it.

	std::list<unsigned> _activationKeys;	/// Keys you can press to activate this Tool. Not necessarily equivalent. Some Tools can be activated in different modes depending on which key you use.
	std::list<unsigned> _activationButtons;	/// Mouse buttons that can be used to activate this Tool.

public:
	Tool();
	virtual ~Tool();

	std::string getName() {return _name;}
	std::string getDescription() {return _description;}

	std::list<unsigned> getActivationKeys()	{return _activationKeys;}
	std::list<unsigned> getActivationButtons()	{return _activationButtons;}


	virtual void onToolSelected()	{}
	virtual void onToolDeselected()	{}

	/// Calls the appropriate handler for the event type.
	virtual void onEvent(Event& event);

protected:
	virtual void onMouseButtonPush(Event& event)	{}
	virtual void onMouseButtonRelease(Event& event)	{}
	virtual void onMouseDrag(Event& event)			{}
	virtual void onMouseMove(Event& event)			{}
	virtual void onKeyDown(Event& event)			{}
	virtual void onKeyUp(Event& event)				{}


};

#endif // TOOL_H
