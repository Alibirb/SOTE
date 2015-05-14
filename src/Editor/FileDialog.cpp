#include "FileDialog.h"


#include "globals.h"

#include "Editor/Editor.h"

/// directory-reading
#include <dirent.h>
#include <sys/stat.h>


#include <stdio.h>
#ifdef _WIN32
    #include <direct.h>
#else
    #include <unistd.h>
#endif


using namespace CEGUI;




FileDialog::FileDialog(std::function<void(FileList)> callOnFinish, bool allowMultipleSelections) : _callOnFinish(callOnFinish), _allowMultipleSelections(allowMultipleSelections)
{
	getEditor()->getCeguiDrawable()->addToDrawOperationQueue(std::bind(&FileDialog::setupWindows, this));
}

FileDialog::~FileDialog()
{
}

void FileDialog::setupWindows()
{
	UVector2 position = CEGUI::UVector2(cegui_reldim(1.0/3), cegui_reldim(0));
	USize size = CEGUI::USize(cegui_reldim(1.0/3), cegui_reldim(1.0/3));

	_frameWindow = static_cast<Window*>(WindowManager::getSingleton().createWindow("EditorLook/FrameWindow"));
	_frameWindow->setPosition(position);
	_frameWindow->setSize(size);
	_frameWindow->setText("File dialog");

	_frameWindow->subscribeEvent(CEGUI::FrameWindow::EventSized, CEGUI::Event::Subscriber(&FileDialog::onResized, this));

	_layout = static_cast<CEGUI::VerticalLayoutContainer*>(CEGUI::WindowManager::getSingleton().createWindow("VerticalLayoutContainer"));
	_frameWindow->addChild(_layout);


	_listbox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/Listbox") );
	_listbox->setSize(CEGUI::USize(UDim(1, 0), UDim(.9, 0)) );
	_listbox->setMaxSize(USize(UDim(1,0), UDim(1, -64.0)));
	_listbox->setSortingEnabled(true);

	_layout->addChild(_listbox);

	_listbox->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&FileDialog::onSelectionChanged, this));


	_editbox = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().createWindow("EditorLook/Editbox") );
	_editbox->setText("");
	_editbox->setMinSize(USize(UDim(1,0), UDim(0,0)));


	_layout->addChild(_editbox);
	_layout->layout();

	char currentDirectory[FILENAME_MAX];
	getcwd(currentDirectory, sizeof(currentDirectory));

	openDirectory(currentDirectory);

	getEditor()->addWindow(_frameWindow);
}


bool FileDialog::onSelectionChanged(const CEGUI::EventArgs& args)
{
	ListboxItem* item = _listbox->getFirstSelectedItem();

	if(item)
	{
		_editbox->setText(item->getText());

		if(item->getText() == "./")
			return true;	/// "./" is the current directory. That's already open, so just ignore it.

		if(item->getText() == "../")
			openParentDirectory();
		else if(isDirectory(item->getText().c_str()))
			openDirectory(_currentPath + item->getText().c_str());
		else
		{
			/// not a directory. File to open.

			/// For the moment, we'll just open the file immediately. Not a nice behavoir, but I'm just testing.
			finalize();
		}
	}
	else
		_editbox->setText("");

	return true;
}


bool FileDialog::onResized(const CEGUI::EventArgs& args)
{
	_layout->layout();
	return true;
}


void FileDialog::openDirectory(std::string path)
{
	struct dirent *entry;
	DIR *dp;

	dp = opendir(path.c_str());
	if (dp == NULL)
	{
		logError("Could not open directory: '" + path + "'.");
		return;
	}

	if(path.at(path.length()-1) != '/')
		path.append("/");

	_listbox->resetList();
	_currentPath = path;
	_frameWindow->setText(_currentPath);

	while((entry = readdir(dp)))
	{
		if(isDirectory(entry->d_name))
		{
			/// Directory. Add a trailing slash to the end to indicate it's a directory.
			ListboxTextItem* item = new ListboxTextItem(entry->d_name + std::string("/"));
			item->setSelectionBrushImage("EditorLook/TextSelectionBrush");
			_listbox->addItem(item);
		}
		else
		{
			ListboxTextItem* item = new ListboxTextItem(entry->d_name);
			item->setSelectionBrushImage("EditorLook/TextSelectionBrush");
			_listbox->addItem(item);
		}
	}

	closedir(dp);
}

void FileDialog::openParentDirectory()
{
	std::string newPath = _currentPath;

	if(newPath.at(newPath.length()-1) == '/')
		newPath = newPath.substr(0, newPath.length()-2);

	int position = newPath.rfind('/');
	if(position == std::string::npos)
		return;		/// Already at the root of the filesystem.

	newPath = newPath.substr(0, position+1);

	openDirectory(newPath);
}

bool FileDialog::isDirectory(std::string filename)
{
	struct stat s;

	std::string fullFilePath = _currentPath + "/" + filename;

	stat(fullFilePath.c_str(), &s);
	return (s.st_mode & S_IFDIR);
}

void FileDialog::finalize()
{
	_selectedFiles.clear();

	ListboxItem* item = _listbox->getFirstSelectedItem();
	while(item != NULL)
	{
		std::string fullFilePath = _currentPath + "/" + item->getText().c_str();
		_selectedFiles.push_back(fullFilePath);
		item = _listbox->getNextSelected(item);
	}

	_callOnFinish(_selectedFiles);

	_frameWindow->hide();
	delete this;
}


