
#include "AngelScriptConsole.h"

#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/ViewerEventHandlers>


const unsigned int MASK_2D = 0xF0000000;

AngelScriptConsole::AngelScriptConsole() : osgWidget::Input("console", "", 50)
{
	osgWidget::WindowManager* wm = new osgWidget::WindowManager(&viewer, 1280.0f, 1024.0f, MASK_2D, osgWidget::WindowManager::WM_PICK_DEBUG);

	osgWidget::Box* box = new osgWidget::Box("vbox", osgWidget::Box::VERTICAL);

	this->setFont("fonts/VeraMono.ttf");
	this->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
	this->setFontSize(15);
	this->setYOffset(this->calculateBestYOffset("y"));
	this->setSize(400.0f, this->getText()->getCharacterHeight());

	box->addWidget(this);
	box->setOrigin(200.0f, 200.0f);

	wm->addChild(box);

	osg::Camera* camera = wm->createParentOrthoCamera();
	root->addChild(camera);

	viewer.addEventHandler(new osgWidget::MouseHandler(wm));
	viewer.addEventHandler(new osgWidget::KeyboardHandler(wm));
	viewer.addEventHandler(new osgWidget::ResizeHandler(wm, camera));

	wm->resizeAllWindows();

}

AngelScriptConsole::~AngelScriptConsole()
{
	//dtor
}

void AngelScriptConsole::enterCommand(std::string code)
{
	getScriptEngine()->eval(code);
}

bool AngelScriptConsole::keyDown(int key, int mask, const osgWidget::WindowManager* wm)
{
	if(!this->active)
		return false;	// Console is not active. Ignore the event.

	osgText::String& s = _text->getText();

	switch (key)
	{
	case osgGA::GUIEventAdapter::KEY_Left:
		if (mask & osgGA::GUIEventAdapter::MODKEY_CTRL)
		{
			bool found = false;
			for (unsigned int i = 0; i < _wordsOffsets.size() - 1; ++i)
			{
				if (_wordsOffsets.at(i) < _index && _index <= _wordsOffsets.at(i+1))
				{
					found = true;
					_index = _wordsOffsets.at(i);
					break;
				}
			}
			if (!found && _wordsOffsets.size())
			{
				_index = _wordsOffsets.at(_wordsOffsets.size()-1);
			}
		}
		else if (_index > 0)
		{
			--_index;
		}
		if (mask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
		{
			_selectionEndIndex = _index;
		}
		else
		{
			_selectionStartIndex = _selectionEndIndex = _index;
		}
		break;
	case osgGA::GUIEventAdapter::KEY_Right:
		if (mask & osgGA::GUIEventAdapter::MODKEY_CTRL)
		{
			bool found = false;
			for (unsigned int i = 0; i < _wordsOffsets.size() - 1; ++i)
			{
				if (_wordsOffsets.at(i) <= _index && _index < _wordsOffsets.at(i+1))
				{
					found = true;
					_index = _wordsOffsets.at(i+1);
					break;
				}
			}
			if (!found && _wordsOffsets.size())
			{
				_index = _wordsOffsets.at(_wordsOffsets.size()-1);
			}
		}
		else if (_index < s.size())
		{
			++_index;
		}

		if (mask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
		{
			_selectionEndIndex = _index;
		}
		else
		{
			_selectionStartIndex = _selectionEndIndex = _index;
		}
		break;
	case osgGA::GUIEventAdapter::KEY_Up:	// History, like in the terminal.
		if(s.createUTF8EncodedString() == "")
		{
			if(history.size() > 0)
			{
				_text->setText(history.front());
				_index = 0;	// reset the index
				_text->update();
				_calculateCursorOffsets();
			}
		}
		break;
	case osgGA::GUIEventAdapter::KEY_Home:
		_index = 0;
		if (mask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
		{
			_selectionEndIndex = _index;
		}
		else
		{
			_selectionStartIndex = _selectionEndIndex = _index;
		}
		break;
	case osgGA::GUIEventAdapter::KEY_End:
		_index = s.size();
		if (mask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
		{
			_selectionEndIndex = _index;
		}
		else
		{
			_selectionStartIndex = _selectionEndIndex = _index;
		}
		break;
	case osgGA::GUIEventAdapter::KEY_Insert:
		_insertMode = !_insertMode;
		break;
	case osgGA::GUIEventAdapter::KEY_Delete:
	{
		unsigned int deleteMin = osg::minimum(_selectionStartIndex,_selectionEndIndex);
		unsigned int deleteMax = osg::maximum(_selectionStartIndex,_selectionEndIndex);

		if (deleteMax - deleteMin > 0)
		{
		}
		else if (mask & osgGA::GUIEventAdapter::MODKEY_CTRL)
		{
			deleteMin = 0;
			deleteMax = 0;
			for (unsigned int i  =0; i < _wordsOffsets.size() - 1; ++i)
			{
				if (_wordsOffsets.at(i) <= _index && _index < _wordsOffsets.at(i+1))
				{
					deleteMin = _wordsOffsets.at(i);
					deleteMax = _wordsOffsets.at(i+1);
					break;
				}
			}
		}
		else if (_index < s.size())
		{
			deleteMin = _index;
			deleteMax = _index + 1;
		}

		if (deleteMin != deleteMax)
			s.erase(s.begin() + deleteMin, s.begin() + deleteMax);

		_text->update();

		_calculateCursorOffsets();

		_index = deleteMin;
		_selectionStartIndex = _selectionEndIndex = _index;
	}
	break;
	case osgGA::GUIEventAdapter::KEY_BackSpace:
	{
		unsigned int deleteMin = osg::minimum(_selectionStartIndex,_selectionEndIndex);
		unsigned int deleteMax = osg::maximum(_selectionStartIndex,_selectionEndIndex);

		if (deleteMax - deleteMin > 0)
		{
		}
		else if(_index >= 1)
		{
			deleteMin = _index - 1;
			deleteMax = _index;
		}

		if (deleteMin != deleteMax)
			s.erase(s.begin() + deleteMin, s.begin() + deleteMax);

		_text->update();

		_calculateCursorOffsets();

		_index = deleteMin;
		_selectionStartIndex = _selectionEndIndex = _index;
	}
	break;


	case osgGA::GUIEventAdapter::KEY_Return:
		enterCommand(s.createUTF8EncodedString());
		history.push_front(s.createUTF8EncodedString());
		_text->setText("");	// Clear the input string
		_index = 0;	// reset the index
		_text->update();
		_calculateCursorOffsets();
		break;


	default:
		if(key > 255 || _index >= _maxSize) return false;
		/*
				if (((key=='v' || key=='V') && (mask & osgGA::GUIEventAdapter::MODKEY_CTRL)) || (key==22))
				{
					std::string data;
		// Data from clipboard
		#ifdef WIN32
					if (::OpenClipboard(NULL))
					{
						HANDLE hData = ::GetClipboardData( CF_TEXT );
						char* buff = (char*)::GlobalLock( hData );
						if (buff) data = buff;
						::GlobalUnlock( hData );
						::CloseClipboard();
					}
		#endif
					if (!data.empty())
					{
						unsigned int deleteMin = osg::minimum(_selectionStartIndex,_selectionEndIndex);
						unsigned int deleteMax = osg::maximum(_selectionStartIndex,_selectionEndIndex);

						if (deleteMax - deleteMin > 0)
						{
							data = data.substr(0, _maxSize-s.size()-(deleteMax - deleteMin));

							s.erase(s.begin() + deleteMin, s.begin() + deleteMax);
							std::copy(data.begin(), data.end(), std::inserter(s, s.begin() + deleteMin));

							_index = deleteMin + data.size();
						}
						else
						{
							data = data.substr(0, _maxSize-s.size());

							std::copy(data.begin(), data.end(), std::inserter(s, s.begin() + _index));
							_index += data.length();
						}

						_selectionStartIndex = _selectionEndIndex = _index;

						_text->update();

						_calculateCursorOffsets();

						_calculateSize(getTextSize());

						getParent()->resize();

						return false;
					}
				}
				else if (((key=='c' || key=='C' || key=='x' || key=='X') && (mask & osgGA::GUIEventAdapter::MODKEY_CTRL)) || (key==3) || (key==24))
				{
					unsigned int selectionMin = osg::minimum(_selectionStartIndex,_selectionEndIndex);
					unsigned int selectionMax = osg::maximum(_selectionStartIndex,_selectionEndIndex);

					if (selectionMax - selectionMin > 0)
					{
						std::string data;
						std::copy(s.begin() + selectionMin, s.begin() + selectionMax, std::inserter(data, data.begin()));

		// Data to clipboard
		#ifdef WIN32
						if(::OpenClipboard(NULL))
						{
							::EmptyClipboard();
							HGLOBAL clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, data.length()+1);
							char* buffer = (char*)::GlobalLock(clipbuffer);
							strcpy(buffer, data.c_str());
							::GlobalUnlock(clipbuffer);
							::SetClipboardData(CF_TEXT,clipbuffer);
							::CloseClipboard();
						}
		#endif

						if (key=='x' || key=='X' || key == 24)
						{
							s.erase(s.begin() + selectionMin, s.begin() + selectionMax);

							_index = selectionMin;

							_selectionStartIndex = _selectionEndIndex = _index;

							_text->update();

							_calculateCursorOffsets();

							_calculateSize(getTextSize());

							getParent()->resize();
						}
					}
					return false;
				}
		*/
		{
			// If something is selected, we need to delete it and insert the character there.
			unsigned int deleteMin = osg::minimum(_selectionStartIndex,_selectionEndIndex);
			unsigned int deleteMax = osg::maximum(_selectionStartIndex,_selectionEndIndex);

			if (deleteMax - deleteMin > 0)
			{
				s.erase(s.begin() + deleteMin, s.begin() + deleteMax);

				_text->update();

				_calculateCursorOffsets();

				_index = deleteMin;
				_selectionStartIndex = _selectionEndIndex = _index;
			}
		}

		if (_insertMode && _index < s.size())
		{
			s[_index] = key;
		}
		else
		{
			if (_index < _maxSize)
				s.insert(s.begin() + _index, key);
		}

		_text->update();

		_calculateCursorOffsets();

		_index++;

		_selectionStartIndex = _selectionEndIndex = _index;
	}

	_calculateSize(getTextSize());

	getParent()->resize();

	return true;
}

void AngelScriptConsole::setActive(bool active)
{
	this->active = active;
	if(active)
	{
		getMainEventHandler()->setInputMode(MainEventHandler::InputMode::Console);
	}
	else
	{
		getMainEventHandler()->setInputMode(MainEventHandler::InputMode::Standard);
	}
}

bool AngelScriptConsole::mouseEnter(double, double, const WindowManager*)
{
	setActive(true);
	return true;
}
bool AngelScriptConsole::mouseLeave(double, double, const WindowManager*)
{
	setActive(false);
	return true;
}

