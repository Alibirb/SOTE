#include "TextEditor.h"

#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/ViewerEventHandlers>

#include "input.h"


#include "globals.h"

#include "Editor.h"



using namespace CEGUI;

TextEditor::TextEditor(int width, int height)
{
	_window = static_cast<Window*>(WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow"));
	_window->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
    _window->setSize(USize(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
    _window->setText("Text editor");

    _editbox = static_cast<MultiLineEditbox*>(CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/MultiLineEditbox"));
	_editbox->setText("Wait a second, wait a second, why am I on trial?\nYou only gave me inches when I need a million miles.");
	_editbox->setPosition( UVector2( UDim( 0.0f, 0.0f ), UDim( 0.0f, 0.0f ) ) );
	_editbox->setSize(CEGUI::USize(CEGUI::UDim(1.0, 0.0), CEGUI::UDim(1.0, 0.0)));
	_editbox->setReadOnly(false);
	_editbox->setWordWrapping(true);

    _window->addChild(_editbox);

	getEditor()->addWindow(_window);
}

TextEditor::~TextEditor()
{
}

bool TextEditor::isActive() {
	return _active;
}
void TextEditor::setActive(bool active) {
	_active = active;
}




/*
												/// Input(name, label, size)
TextEditor::TextEditor(int width, int height) : osgWidget::Input("text editor", "", 0xFFFFFFFFFFFFFFFF)
{
	//this->setFont("fonts/VeraMono.ttf");
	this->setFont("fonts/arial.ttf");
	this->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
	this->setFontSize(15);
	this->setYOffset(this->calculateBestYOffset());
	this->setSize(width, height);
}

TextEditor::~TextEditor()
{
	//dtor
}

bool TextEditor::keyDown(int key, int mask, const osgWidget::WindowManager* wm)
{
	if(!this->_active)
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


	default:
		if((key > 255 && key != osgGA::GUIEventAdapter::KEY_Return) || _index >= _maxSize) return false;
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
		/*

		if(key == osgGA::GUIEventAdapter::KEY_Return)
			key = '\n';


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
}*/



/*
bool TextEditor::mouseEnter(double, double, const osgWidget::WindowManager*)
{
	setActive(true);
	return true;
}

bool TextEditor::mouseLeave(double, double, const osgWidget::WindowManager*)
{
	setActive(false);
	return true;
}
*/
