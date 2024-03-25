#pragma once

#include <io.h>
#include "Constants.h"
#include "IDataAccess.h"
#include "sqlite3.h"


class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess() = default;
	virtual ~DatabaseAccess();

	// album related
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	void printAlbums() override;

	// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void removePicture(const int pictureID);
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void removePictureTags(const int pictureID);

	// user related
	void printUsers() override;
	User getUser(int userId) override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;

	// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	// DB
	bool initDatabase();
	bool open() override;
	void close() override;
	void clear() override;

	bool executeSqlStatement(const std::string& statement);
	template<typename funcPtr>
	bool executeSqlQuery(const std::string& query, const funcPtr callbackFunction, void* callbackParam);

	// Get info from DB
	static int getNextUserIDCallback(void* data, int argc, char** argv, char** azColName);
	int getNextUserID() override;

	static int getAlbumIDCallback(void* data, int argc, char** argv, char** azColName);
	int getAlbumID(const std::string& albumName, int userId);
	int getAlbumID(const std::string& albumName);

	static int getPictureIDCallback(void* data, int argc, char** argv, char** azColName);
	int getPictureID(const std::string& pictureName, int albumID);

	// Get DB info in structure
	static int getAlbumPicturesCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Record> getAlbumPictures(const int albumID);


private:
	sqlite3* _DB;

	std::list<Album> _albums;
	std::list<User> _users;
};
