#ifndef INPUT_H
#define INPUT_H

#include <osgGA/GUIEventHandler>

class MainEventHandler : public osgGA::GUIEventHandler
{
public:
	enum InputMode{
		Standard,	// Game is in play. WASD move player, etc.
		Console,
		Menu
	};
private:
	InputMode _inputMode = InputMode::Standard;
public:
	std::map<int, bool> keyState;	// OSG uses an enum for keys. Values for 0-9 and a-z are set equal to the character (KEY_A = 'a')

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void setInputMode(InputMode newMode) {
		_inputMode = newMode;
	}

};

MainEventHandler* getMainEventHandler();

#endif // INPUT_H
