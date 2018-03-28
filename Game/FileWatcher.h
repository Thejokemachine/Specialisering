#pragma once
#include <functional>
#include <windows.h>
#include <thread>
#include <mutex>
#include <vector>
#include <map>

namespace CommonUtilities
{
	typedef std::function<void(const std::wstring&)> callback_function_file;
	typedef std::function<void()> callback_function_directory;
	class FileWatcher
	{
	public:
		~FileWatcher();

		/* Will check the file for includes and add them as well*/
		bool WatchFileChangeWithDependencies(std::wstring aFile, callback_function_file aFunctionToCallOnChange);
		bool WatchFileChange(std::wstring aFile, callback_function_file aFunctionToCallOnChange);
		bool WatchDirectoryChange(std::wstring aDirectory, callback_function_directory aFunctionToCallOnChange);

		void FlushChanges();

		static FileWatcher& GetInstance();
	private:
		FileWatcher();
		void OnFolderChange(const std::wstring& aDir);
		bool WatchDirectory(const std::wstring& aDir);
		void OnFileChange(std::wstring& aFile);
		void OnDirectoryChange(const std::wstring& aFile);

		void GetAllFilesInFolder(std::wstring aDir, std::vector<WIN32_FIND_DATAW>& out);

		std::thread myThread;

		typedef std::map<std::wstring, std::vector<WIN32_FIND_DATAW>> FolderMap;
		FolderMap myFolders;

		std::vector<std::wstring> myFileChangedThreaded;
		std::vector<std::wstring> myFileChanged;
		std::vector<std::wstring> myDirectoriesChangedThreaded;
		std::vector<std::wstring> myDirectoriesChanged;
		std::map<std::wstring, std::vector<callback_function_file> > myCallbacks;
		std::map<std::wstring, std::vector<callback_function_directory>> myDirectoryCallbacks;

		typedef std::map<std::wstring, std::vector<std::wstring>> Dependencies;
		Dependencies myDependencies;

		std::vector<std::wstring> myThreadedFoldersToWatch;

		std::mutex myMutex;
		std::mutex myAddNewFolderMutex;
		bool myShouldEndThread;
		bool myThreadIsDone;
	};

} 
namespace CU = CommonUtilities;