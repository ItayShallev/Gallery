#include "DatabaseAccess.h"

#define USERS_TABLE_SQL_STATEMENT R"(CREATE TABLE USERS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
NAME TEXT NOT NULL);)"

#define ALBUMS_TABLE_SQL_STATEMENT R"(CREATE TABLE ALBUMS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
NAME TEXT NOT NULL,
CREATION_DATE DATE NOT NULL,
USER_ID INTEGER NOT NULL,
FOREIGN KEY (USER_ID) REFERENCES USERS(ID));)"

#define PICTURES_TABLE_SQL_STATEMENT R"(CREATE TABLE PICTURES(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
NAME TEXT NOT NULL,
LOCATION TEXT NOT NULL,
CREATION_DATE DATE NOT NULL,
ALBUM_ID INTEGER NOT NULL,
FOREIGN KEY (ALBUM_ID) REFERENCES ALBUMS(ID));)"

#define TAGS_TABLE_SQL_STATEMENT R"(CREATE TABLE TAGS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
PICTURE_ID INTEGER NOT NULL,
USER_ID INTEGER NOT NULL,
FOREIGN KEY (PICTURE_ID) REFERENCES PICTURES(ID),
FOREIGN KEY (USER_ID) REFERENCES USERS(ID));)"

// DEBUG
#define INSERT_TO_USERS R"(INSERT INTO USERS (NAME) VALUES ('User1'), ('User2'), ('User3'), ('User4'), ('User5');)"

#define INSERT_TO_ALBUMS R"(INSERT INTO ALBUMS
(NAME, CREATION_DATE, USER_ID)
VALUES ('Album1', '01/01/2001', 1), ('Album2', '02/02/2002', 2), ('Album3', '03/03/2003', 3), ('Album4', '04/04/2004', 4), ('Album5', '05/05/2005', 5);)"

#define INSERT_INTO_PICTURES R"(INSERT INTO PICTURES
(NAME, LOCATION, CREATION_DATE, ALBUM_ID)
VALUES ('Picture1', '/images', '01/01/2001', 1),
('Picture2', '/images', '02/02/2002', 2),
('Picture3', '/images', '03/03/2003', 3),
('Picture4', '/images', '04/04/2004', 4),
('Picture5', '/images', '05/05/2005', 5);)"
// DEBUG


DatabaseAccess::~DatabaseAccess()
{
	sqlite3_close(this->_DB);
}


// ******************* Album *******************

const std::list<Album> DatabaseAccess::getAlbums()
{
	return std::list<Album>();
}


const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	return std::list<Album>();
}


void DatabaseAccess::createAlbum(const Album& album)
{
}


void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
}


bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	return false;
}


Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	return Album();
}

/**
 @brief			Closes the opened album
 @return		Void
 @note			Does nothing since the "openedAlbum" member is at the AlbumManager class
 */
void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	// AlbumManager takes care of closing the album
}


void DatabaseAccess::printAlbums()
{
}


// ******************* Picture *******************

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
}


void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
}


/**
 @brief		Tags a user in a picture
 @param     albumName       The name of the album that contains the picture to tag the user in
 @param		pictureName		The name of the picture to tag the user in
 @param     userId          The id of the user to tag in the picture
 @return	Void
 */
void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumID = getAlbumID(albumName);
	int pictureID = getPictureID(pictureName, albumID);

	std::string insertToTags = R"(
					INSERT INTO TAGS
					(PICTURE_ID, USER_ID)
					VALUES ()" + std::to_string(pictureID) + ", " + std::to_string(userId) + ")";

	executeSqlStatement(insertToTags);
}


void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
}


// ******************* Users *******************

void DatabaseAccess::printUsers()
{
}


User DatabaseAccess::getUser(int userId)
{
	return User(0, "Itay");
}


void DatabaseAccess::createUser(User& user)
{
}


void DatabaseAccess::deleteUser(const User& user)
{
}


bool DatabaseAccess::doesUserExists(int userId)
{
	return false;
}


// ******************* Users Statistics *******************

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	return 0;
}


int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	return 0;
}


int DatabaseAccess::countTagsOfUser(const User& user)
{
	return 0;
}


float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	return 0.0f;
}

/**
 @brief
 @param
 @return
 */
// ******************* Queries *******************

User DatabaseAccess::getTopTaggedUser()
{
	return User(0, "Itay");
}


Picture DatabaseAccess::getTopTaggedPicture()
{
	return Picture(0, "Itay's pic");
}


std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	return std::list<Picture>();
}


// ******************* Database *******************

/**
 @brief			Initializes the Gallery database by creating necessary tables if they do not exist.
 @return		True if the database is successfully initialized, false otherwise
 */
bool DatabaseAccess::initDatabase()
{
	std::string initStatement = std::string(USERS_TABLE_SQL_STATEMENT) + ALBUMS_TABLE_SQL_STATEMENT + PICTURES_TABLE_SQL_STATEMENT + TAGS_TABLE_SQL_STATEMENT;

	if (!this->executeSqlStatement(initStatement)) { return false; }

	std::string insertToTables = std::string(INSERT_TO_USERS) + std::string(INSERT_TO_ALBUMS) + std::string(INSERT_INTO_PICTURES);

	if (!this->executeSqlStatement(insertToTables)) { return false; }

	return true;
}


/**
 @brief			Opens the Gallery database.
 @return		True if the database is successfully opened or initialized, false otherwise
 */
bool DatabaseAccess::open()
{
	// Checking if the file exists before trying to open it
	int doesFileExist = _access(DB_FILE_NAME.c_str(), 0);

	// Opening the DB
	int res = sqlite3_open(DB_FILE_NAME.c_str(), &this->_DB);
	if (res != SQLITE_OK)
	{
		this->_DB = nullptr;
		std::cout << "Failed to open DB" << std::endl;

		return false;
	}

	// Initializing the DB if the DB doesn't exist
	if (doesFileExist != DATABASE_EXISTS)
	{
		if (!initDatabase()) { return false; }
	}

	return true;
}


/**
 @brief			Closes the Gallery database
 @return		Void
 */
void DatabaseAccess::close()
{
	sqlite3_close(this->_DB);
	this->_DB = nullptr;
}


/**
 @brief			Clears the data that stored in the class
 @return		Void
 */
void DatabaseAccess::clear()
{
	this->_albums.clear();
	this->_users.clear();
}


/**
 @brief		Executes a given SQL statement on a given Database
 @param		statement		The statement to execute
 @return	A boolean value indicating whether the operation succeeded or not
 */
bool DatabaseAccess::executeSqlStatement(const std::string& statement)
{
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_DB, statement.c_str(), nullptr, nullptr, &errMessage);

	if (res != SQLITE_OK)
	{
		sqlite3_close(this->_DB);
		this->_DB = nullptr;

		std::cout << "Failed to send the query: " << std::endl << errMessage << std::endl;

		return false;
	}

	return true;
}


/**
 @brief		Executes an SQL query on a SQLite database with a callback function for result rows
 @param		query				The query to execute
 @param		callbackFunction	The callback function for each result row
 @param		callbackParam		The parameter for the callback function (The way of transmiting the SQL query result)
 @note		callbackFunction and callbackParam can be nullptrs
 @return	A boolean value indicating whether the operation succeeded or not
 */
template<typename funcPtr>
bool DatabaseAccess::executeSqlQuery(const std::string& query, const funcPtr callbackFunction, void* callbackParam)
{
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_DB, query.c_str(), callbackFunction, callbackParam, &errMessage);

	if (res != SQLITE_OK)
	{
		sqlite3_close(this->_DB);
		this->_DB = nullptr;

		std::cout << "Failed to send the query: " << std::endl << errMessage << std::endl;

		return false;
	}

	return true;
}


// ******************* Get Info *******************


/**
 @brief		Callback function for getting the ID of an album by its name and owner
 @param		data			A pointer to an integer where the retrieved Album ID will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getAlbumIDCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<int*>(data)) = std::atoi(argv[0]);		// Returning the Album ID by storing it inside data (sqlite3_exec 4th argument)

	return 0;
}


/**
 @brief		Gets an ID of an album by its name and owner
 @param		albumName		The name of the album to get its ID
 @param		userID			The ID of the owner user of the album to get its ID
 @return	The ID of the specified album
 */
int DatabaseAccess::getAlbumID(const std::string& albumName, int userId)
{
	std::string selectQuery = R"(
					BEGIN TRANSACTION;

					SELECT ID FROM ALBUMS
					WHERE NAME = ')" + albumName + "' AND USER_ID = " + std::to_string(userId) + R"(;
					
					END TRANSACTION;
					)";

	int albumID = -1;
	if (!executeSqlQuery(selectQuery, getAlbumIDCallback, &albumID)) { return -1; }

	return albumID;
}

int DatabaseAccess::getAlbumID(const std::string& albumName)
{
	std::string selectQuery = R"(
					BEGIN TRANSACTION;

					SELECT ID FROM ALBUMS
					WHERE NAME = ')" + albumName + R"(';
					
					END TRANSACTION;
					)";

	int albumID = -1;
	if (!executeSqlQuery(selectQuery, getAlbumIDCallback, &albumID)) { return -1; }

	return albumID;
}


/**
 @brief		Callback function for getting the ID of a picture by its name and album ID
 @param		data			A pointer to an integer where the retrieved Picture ID will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	The ID of the specified album
 */
int DatabaseAccess::getPictureIDCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<int*>(data)) = std::atoi(argv[0]);		// Returning the Picture ID by storing it inside data (sqlite3_exec 4th argument)

	return 0;
}


/**
 @brief		Gets an ID of a picture by its name and album ID
 @param		pictureName		The name of the picture to get its ID
 @param		albumID			The ID of the album that the picture belongs to to get its ID
 @return	The ID of the specified picture
 */
int DatabaseAccess::getPictureID(const std::string& pictureName, int albumID)
{
	std::string selectQuery = R"(
					BEGIN TRANSACTION;

					SELECT ID FROM PICTURES
					WHERE NAME = ')" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumID) + R"(;
					
					END TRANSACTION;
					)";

	int pictureID = -1;
	if (!executeSqlQuery(selectQuery, getPictureIDCallback, &pictureID)) { return -1; }

	return pictureID;
}
