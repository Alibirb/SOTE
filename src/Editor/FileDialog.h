#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "CEGUIStuff.h"

#include <functional>


/// A file-selection dialog. For both loading and saving files.
class FileDialog
{
public:
	typedef std::vector<std::string> FileList;

protected:
	CEGUI::Window* _frameWindow;
	CEGUI::VerticalLayoutContainer* _layout;
	CEGUI::Editbox* _editbox;
	CEGUI::Listbox* _listbox;
	std::string _currentPath;
	FileList _selectedFiles;
	bool _allowMultipleSelections;

	std::function<void(FileList)> _callOnFinish;	/// Function to call once the selection is made (typically to whatever opened the dialog)

public:
	FileDialog(std::function<void(FileList)> callOnFinish, bool allowMultipleSelections = false);
	virtual ~FileDialog();

	void setupWindows();

protected:
	bool onSelectionChanged(const CEGUI::EventArgs& args);
	bool onResized(const CEGUI::EventArgs& args);

	void openDirectory(std::string path);
	void openParentDirectory();

	bool isDirectory(std::string filename);	/// Returns whether the file named "filename" (in currently-open directory) is a directory.

	void finalize();
};

#endif // FILEDIALOG_H

