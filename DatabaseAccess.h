#pragma once

#include <io.h>
#include "Constants.h"
#include "IDataAccess.h"
#include "sqlite3.h"


class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess();
	virtual ~DatabaseAccess();

	// ****************************** Album ******************************
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;

	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;

	static int doesAlbumExistsCallback(void* data, int argc, char** argv, char** azColName);
	bool doesAlbumExists(const std::string& albumName, int userId) override;

	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;

	void printAlbums() override;

	// ****************************** Picture ******************************
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void removePicture(const int pictureID);

	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void removePictureTags(const int pictureID);

	// ****************************** User ******************************
	void printUsers() override;
	User getUser(int userId) override;

	void createUser(User& user) override;
	void deleteUser(const User& user) override;

	static int doesUserExistsCallback(void* data, int argc, char** argv, char** azColName);
	bool doesUserExists(int userId) override;

	// ****************************** User Statistics ******************************
	static int countAlbumsOwnedOfUserCallback(void* data, int argc, char** argv, char** azColName);
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// ****************************** Query ******************************
	static int getTopTaggedUserCallback(void* data, int argc, char** argv, char** azColName);
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	// ****************************** DB ******************************
	bool initDatabase();
	bool open() override;
	void close() override;
	void clear() override;

	bool executeSqlStatement(const std::string& statement);
	template<typename funcPtr>
	bool executeSqlQuery(const std::string& query, const funcPtr callbackFunction, void* callbackParam);

	// ****************************** Get Info From DB ******************************
	static int getNextIDCallback(void* data, int argc, char** argv, char** azColName);
	int getNextUserID() override;
	int getNextPictureID() override;

	static int getUserNameCallback(void* data, int argc, char** argv, char** azColName);
	std::string getUserName(const int userID);

	static int getAlbumIDCallback(void* data, int argc, char** argv, char** azColName);
	int getAlbumID(const std::string& albumName, int userId);
	int getAlbumID(const std::string& albumName);

	static int getPictureIDCallback(void* data, int argc, char** argv, char** azColName);
	int getPictureID(const std::string& pictureName, int albumID);

	// ****************************** Get DB Records In A Structure ******************************
	static int getAlbumsRecordsCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Record> getAlbumsRecords();
	std::list<Record> getUserAlbumsRecords(const int userID);
	std::list<Record> getAlbumRecordsByName(const std::string albumName);
	std::list<Album> buildAlbumsList(const std::list<Record>& albumRecords);

	static int getAlbumPicturesRecordsCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Record> getAlbumPicturesRecords(const int albumID);

	static int getPictureTagsRecordsCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Record> getPictureTagsRecords(const int pictureID);

	static int getUsersRecordsCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Record> getUsersRecords();


private:
	sqlite3* _DB;
};
