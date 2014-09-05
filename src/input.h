#ifndef INPUT_H
#define INPUT_H

#include <osgGA/GUIEventHandler>

#include <iostream>


/// TODO: key bindings should be stored in a struct/class. Basically a state machine (but not using the StateMachine class)


class MainEventHandler : public osgGA::GUIEventHandler
{
public:
	enum InputMode{
		Standard,   /// Game is in play. WASD move player, etc.
		Console, 	/// AngelScript console	// FIXME: the console should probably just be part of the editor
		Editor,     /// Game editor.
		Menu        /// in-game menu
	};
private:
	InputMode _inputMode = InputMode::Standard;
public:
	std::map<int, bool> keyState;	// OSG uses an enum for keys. Values for 0-9 and a-z are set equal to the character (KEY_A = 'a')

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void setInputMode(InputMode newMode);

};

MainEventHandler* getMainEventHandler();

#endif // INPUT_H
