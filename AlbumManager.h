#pragma once
#include <Windows.h>
#include <vector>
#include <limits>
#include "Constants.h"
#include "MemoryAccess.h"
#include "DatabaseAccess.h"
#include "Album.h"


class AlbumManager
{
public:
	AlbumManager(IDataAccess& dataAccess);

	void executeCommand(CommandType command);
	void printHelp() const;

	using handler_func_t = void (AlbumManager::*)(void);    

private:
    int m_nextAlbumId{};
    int m_nextPictureId{};
    int m_nextUserId{};
    std::string m_currentAlbumName{};
	IDataAccess& m_dataAccess;
	Album m_openAlbum;

	void help();
	// albums management
	void createAlbum();
	void openAlbum();
	void closeAlbum();
	void deleteAlbum();
	void listAlbums();
	void listAlbumsOfUser();

	// Picture management
	void addPictureToAlbum();
	void removePictureFromAlbum();
	void listPicturesInAlbum();
	void showPicture();

	void openPictureInApp(const std::string app, const Picture& pic);

	// tags related
	void tagUserInPicture();
	void untagUserInPicture();
	void listUserTags();

	// users management
	void addUser();
	void removeUser();
	void listUsers();
	void userStatistics();

	void topTaggedUser();
	void topTaggedPicture();
	void picturesTaggedUser();
	void exit();

	std::string getInputFromConsole(const std::string& message);
	bool fileExistsOnDisk(const std::string& filename);
	void refreshOpenAlbum();
    bool isCurrentAlbumSet() const;

	// General
	int getValidChoice(const std::string& message, const std::string& errorMessage, const int& min, const int& max);
	void clearBuffer();

	static const std::vector<struct CommandGroup> m_prompts;
	static const std::map<CommandType, handler_func_t> m_commands;
};
