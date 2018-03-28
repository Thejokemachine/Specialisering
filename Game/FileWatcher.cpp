#include "FileWatcher.h"
#include <fstream>
#include <string>
using namespace CommonUtilities;

FileWatcher::FileWatcher() :
	  myShouldEndThread(false)
	, myThreadIsDone(false)

{
	myThread = std::thread(
		[&]
	{
		while (!myShouldEndThread)
		{
			myMutex.lock();
			myAddNewFolderMutex.lock();
			for (std::wstring& folder : myThreadedFoldersToWatch)
			{
				OnFolderChange(folder);
			}

			myMutex.unlock();
			myAddNewFolderMutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		myThreadIsDone = true;
	});
}


FileWatcher::~FileWatcher()
{
	myShouldEndThread = true;
	while (!myThreadIsDone)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
	myThread.join();
}


void FileWatcher::FlushChanges()
{
	std::lock_guard<std::mutex> guard(myMutex);


	myFileChanged.swap(myFileChangedThreaded);

	for (std::wstring& theString : myFileChanged)
	{
		std::wstring directoryOfFile(theString);
		directoryOfFile = directoryOfFile.substr(0, directoryOfFile.find_last_of(L"\\/"));

		std::wstring theFile(theString);
		theFile = theFile.substr(theFile.find_first_of(L"\\/") + 1, theFile.size());

		std::vector<callback_function_file> callbacks = myCallbacks[theFile];
		for (unsigned int i = 0; i < callbacks.size(); i++)
		{
			if (callbacks[i])
			{
				callbacks[i](theString);
			}
		}
	}


	myFileChanged.clear();

	myDirectoriesChanged.swap(myDirectoriesChangedThreaded);

	for (std::wstring& theString : myDirectoriesChanged)
	{
		std::vector<callback_function_directory> callbacks = myDirectoryCallbacks[theString];
		for (unsigned int i = 0; i < callbacks.size(); i++)
		{
			if (callbacks[i])
			{
				callbacks[i]();
			}
		}
	}


	myDirectoriesChanged.clear();

}

FileWatcher & CommonUtilities::FileWatcher::GetInstance()
{
	static FileWatcher instance;
	return instance;
}

void FileWatcher::OnFolderChange(const std::wstring& aDir)
{
	std::vector<WIN32_FIND_DATAW> currentFolderFiles;
	GetAllFilesInFolder(aDir, currentFolderFiles);
	std::vector<WIN32_FIND_DATAW>& savedFolderFiles = myFolders[aDir];
	if (savedFolderFiles.size() != currentFolderFiles.size())
	{
		auto dirCallbackIt = myDirectoryCallbacks.find(aDir);
		if (dirCallbackIt != myDirectoryCallbacks.end())
		{
			OnDirectoryChange(aDir);
		}
		savedFolderFiles = currentFolderFiles;
	}
	for (WIN32_FIND_DATAW& currentFile : currentFolderFiles)
	{
		for (WIN32_FIND_DATAW& savedFile : savedFolderFiles)
		{
			if (std::wstring(currentFile.cFileName).compare(savedFile.cFileName) == 0)
			{
				ULARGE_INTEGER currentFileTime;
				SYSTEMTIME currentFileTimeSystem;
				FileTimeToSystemTime(&currentFile.ftLastWriteTime, &currentFileTimeSystem);
				currentFileTime.LowPart = currentFile.ftLastWriteTime.dwLowDateTime;
				currentFileTime.HighPart = currentFile.ftLastWriteTime.dwHighDateTime;
				__int64 currentFileTime64 = currentFileTime.QuadPart;

				ULARGE_INTEGER savedFileTime;
				SYSTEMTIME savedFileTimeSystem;
				FileTimeToSystemTime(&savedFile.ftLastWriteTime, &savedFileTimeSystem);
				savedFileTime.LowPart = savedFile.ftLastWriteTime.dwLowDateTime;
				savedFileTime.HighPart = savedFile.ftLastWriteTime.dwHighDateTime;
				__int64 savedFileTime64 = savedFileTime.QuadPart;

				if (currentFileTime64 != savedFileTime64)
				{
					std::wstring fileThatChangedPath = aDir + L"\\" + std::wstring(currentFile.cFileName);
					bool isDependency = myDependencies.find(fileThatChangedPath) != myDependencies.end();
					if (isDependency)
					{
						std::vector<std::wstring>& deps = myDependencies[fileThatChangedPath];
						for (std::wstring& file : deps)
						{
							OnFileChange(file);
						}

					}
					else
					{
						OnFileChange(fileThatChangedPath);
					}

					// We have a change
					auto dirCallbackIt = myDirectoryCallbacks.find(aDir);
					if (dirCallbackIt != myDirectoryCallbacks.end())
					{
						OnDirectoryChange(aDir);
					}
					savedFile = currentFile;
				}
			}
		}
	}
}

void FileWatcher::OnFileChange(std::wstring& aFile)
{
	for (unsigned int i = 0; i < myFileChangedThreaded.size(); i++)
	{
		if (myFileChangedThreaded[i].compare(aFile) == 0)
		{
			return;
		}
	}
	myFileChangedThreaded.push_back(aFile);

}

void CommonUtilities::FileWatcher::OnDirectoryChange(const std::wstring & aFile)
{
	for (unsigned int i = 0; i < myDirectoriesChangedThreaded.size(); i++)
	{
		if (myDirectoriesChangedThreaded[i].compare(aFile) == 0)
		{
			return;
		}
		}
	myDirectoriesChangedThreaded.push_back(aFile);
}

bool FileWatcher::WatchFileChangeWithDependencies(std::wstring aFile, callback_function_file aFunctionToCallOnChange)
{
#ifdef _RETAIL
		return false;
#else
	std::ifstream stream(aFile);
	if (!stream.good())
	{
		stream.close();
		return false;
	}

	std::wstring directoryOfFile(aFile);
	directoryOfFile = directoryOfFile.substr(0, directoryOfFile.find_last_of(L"\\/"));

	std::string line;
	const std::string includeString = "include";
	while (getline(stream, line))
	{
		std::size_t found = line.find(includeString);
		if (found != std::string::npos)
		{
			std::string foundFile(line);
			foundFile = foundFile.substr(foundFile.find_first_of("\"") + 1, foundFile.size());
			foundFile = foundFile.substr(0, foundFile.find_last_of("\""));
			if (!foundFile.empty())
			{
				std::wstring depFile = directoryOfFile + L"\\" + std::wstring(foundFile.begin(), foundFile.end());
				WatchFileChange(depFile, aFunctionToCallOnChange);
				myDependencies[depFile].push_back(aFile);
			}
		}
	}

	stream.close();
	return WatchFileChange(aFile, aFunctionToCallOnChange);
#endif
}

bool FileWatcher::WatchFileChange(std::wstring aFile, callback_function_file aFunctionToCallOnChange)
{
#ifdef _RETAIL
		return false;
#else
	std::ifstream stream(aFile);
	if (!stream.good())
	{
		stream.close();
		return false;
	}
	stream.close();

	std::wstring directoryOfFile(aFile);
	directoryOfFile = directoryOfFile.substr(0, directoryOfFile.find_last_of(L"\\/"));

	std::wstring theFile(aFile);
	theFile = theFile.substr(theFile.find_first_of(L"\\/") + 1, theFile.size());

	myCallbacks[theFile].clear();
	myCallbacks[theFile].push_back(aFunctionToCallOnChange);
	return WatchDirectory(directoryOfFile);
#endif
}

bool CommonUtilities::FileWatcher::WatchDirectoryChange(std::wstring aDirectory, callback_function_directory aFunctionToCallOnChange)
{
#ifdef _RETAIL
	return false;
#else
	myDirectoryCallbacks[aDirectory].push_back(aFunctionToCallOnChange);
	return WatchDirectory(aDirectory);
#endif
}

bool FileWatcher::WatchDirectory(const std::wstring& aDir)
{
	FolderMap::iterator iter = myFolders.find(aDir);
	if (iter != myFolders.end())
	{
		//Already in our watch list
		return true;
	}

	auto& it = myFolders[aDir];
	it.clear();
	GetAllFilesInFolder(aDir, it);

	if (myThreadedFoldersToWatch.size() != 0)
	{
		myAddNewFolderMutex.lock();
		myThreadedFoldersToWatch.push_back(aDir);
		myAddNewFolderMutex.unlock();
	}
	else
	{
		myThreadedFoldersToWatch.push_back(aDir);
	}

	return true;
}

void FileWatcher::GetAllFilesInFolder(std::wstring aDir, std::vector<WIN32_FIND_DATAW>& out)
{
	aDir += L"/*.*";
	WIN32_FIND_DATAW fd;

	HANDLE hFind = ::FindFirstFileW(aDir.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				out.push_back(fd);
			}
		} while (::FindNextFileW(hFind, &fd));
		::FindClose(hFind);
	}
}
