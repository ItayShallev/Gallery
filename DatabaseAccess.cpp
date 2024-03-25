#include "DatabaseAccess.h"


DatabaseAccess::~DatabaseAccess()
{
	sqlite3_close(this->_DB);
}




// ********************************************************* Album *********************************************************

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


/**
 @brief		Removes the album from the database
 @param     albumName		The name of the album to remove
 @param     userID			The ID of the user who owns the album
 @return	Void
 @note		The album removal removes also all the album-related information from the database (including Pictures and tags)
 */
void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	int albumID = this->getAlbumID(albumName);

	// Creating a list of all the pictures in the album
	std::list<Record> albumPictures = this->getAlbumPictures(albumID);

	// Iterating over the pictures list, and removing all album inner data (Picture-Tags and Pictures) before removing the album itself
	for (auto pictureIterator = albumPictures.begin(); pictureIterator != albumPictures.end(); ++pictureIterator)
	{
		this->removePictureTags(std::stoi(pictureIterator->at("ID")));
		this->removePicture(std::stoi(pictureIterator->at("ID")));
	}

	// Removing the album itself
	std::string deleteAlbumStatement = R"(
                    BEGIN TRANSACTION;
					
				    DELETE FROM ALBUMS
					WHERE ID = )" + std::to_string(albumID) + R"(;
					
                    END TRANSACTION;
                    )";

	executeSqlStatement(deleteAlbumStatement);
}


bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	return true;
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




// ********************************************************* Picture *********************************************************

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
}


void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
}


/**
 @brief		Removes a picture from the DB
 @param		pictureID		The ID of the picture to remove from the DB
 @return	Void
 */
void DatabaseAccess::removePicture(const int pictureID)
{
	std::string removePictureStatement = R"(
					BEGIN TRANSACTION;
					
					DELETE FROM PICTURES
					WHERE ID = )" + std::to_string(pictureID) + R"(;
					
					END TRANSACTION;
					)";

	executeSqlStatement(removePictureStatement);
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
					BEGIN TRANSACTION;
					
					INSERT INTO TAGS
					(PICTURE_ID, USER_ID)
					VALUES ()" + std::to_string(pictureID) + ", " + std::to_string(userId) + ")" + R"(;
					
					END TRANSACTION;
					)";

	std::cout << executeSqlStatement(insertToTags);
}


/**
 @brief		Untags a user in a picture
 @param     albumName       The name of the album that contains the picture to untag the user from
 @param		pictureName		The name of the picture to untag the user from
 @param     userId          The id of the user to untag from the picture
 @return	Void
 */
void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumID = getAlbumID(albumName);
	int pictureID = getPictureID(pictureName, albumID);

	std::string deleteFromTags = R"(
					BEGIN TRANSACTION;
					
                    DELETE FROM TAGS
                    WHERE PICTURE_ID = )" + std::to_string(pictureID) + " AND USER_ID = " + std::to_string(userId) + R"(;
					
					END TRANSACTION;
					)";
	
	executeSqlStatement(deleteFromTags);
}


/**
 @brief		Removes all tags in the given picture from Tags table
 @param		pictureID		The id of the picture to remove all tags from
 @return	Void
 */
void DatabaseAccess::removePictureTags(const int pictureID)
{
	std::string deleteFromTagsStatement = R"(
                    BEGIN TRANSACTION;
                    
                    DELETE FROM TAGS
                    WHERE PICTURE_ID = )" + std::to_string(pictureID) + R"(;
                    
                    END TRANSACTION;
                    )";

	executeSqlStatement(deleteFromTagsStatement);
}




// ********************************************************* Users *********************************************************

void DatabaseAccess::printUsers()
{
}


User DatabaseAccess::getUser(int userId)
{
	return User(userId, "");
}


/**
 @brief			Creates a new user
 @param			The user to create in the Users table
 @return		Void
 */
void DatabaseAccess::createUser(User& user)
{
	std::string createUserStatement = R"(
		BEGIN TRANSACTION;

		INSERT INTO USERS
		(ID, NAME)
		VALUES ()" + std::to_string(user.getId()) + ", '" + user.getName() + R"(');
		
		END TRANSACTION;
		)";

	executeSqlStatement(createUserStatement);
}


/**
 @brief			Delete an existing user from the database
 @param			The user to delete in the Users table
 @return		Void
 */
void DatabaseAccess::deleteUser(const User& user)
{
	std::string deleteUserStatement = R"(
					BEGIN TRANSACTION;
					
					DELETE FROM USERS
					WHERE ID = )" + std::to_string(user.getId()) + R"(;
					
				    END TRANSACTION;
					)";

	executeSqlStatement(deleteUserStatement);
}


bool DatabaseAccess::doesUserExists(int userId)
{
	return true;
}




// ********************************************************* Users Statistics *********************************************************

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




// ********************************************************* Queries *********************************************************

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




// ********************************************************* Database *********************************************************

/**
 @brief			Initializes the Gallery database by creating necessary tables if they do not exist.
 @return		True if the database is successfully initialized, false otherwise
 */
bool DatabaseAccess::initDatabase()
{
	std::string initStatement = std::string(USERS_TABLE_SQL_STATEMENT) + ALBUMS_TABLE_SQL_STATEMENT + PICTURES_TABLE_SQL_STATEMENT + TAGS_TABLE_SQL_STATEMENT;

	if (!this->executeSqlStatement(initStatement)) { return false; }

	std::string insertToTables = std::string(INSERT_TO_USERS) + std::string(INSERT_TO_ALBUMS) + std::string(INSERT_INTO_PICTURES) + std::string(INSERT_TO_TAGS);

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
 @brief		Executes an SQL query on an SQLite database
 @param		query				The query to execute
 @param		callbackFunction	The callback function to call for each result row
 @param		callbackParam		The parameter for the callback function (The way of transmiting the SQL query result)
 @return	A boolean value indicating whether the operation succeeded or not
 @note		callbackFunction and callbackParam can be nullptrs
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




// ********************************************************* Get Info *********************************************************

/**
 @brief		Callback function for getting the next user ID from the USERS-table-SELECT-query-response
 @param		data			A pointer to an integer where the retrieved ID will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getNextUserIDCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<int*>(data)) = std::stoi(argv[0]);

	return 0;
}


/**
 @brief		Returns the next user ID to create for a new user
 @return	The next user ID to create for a new user
 */
int DatabaseAccess::getNextUserID()
{
	std::string selectQuery = R"(
					SELECT ID FROM USERS
	                ORDER BY ID DESC
					LIMIT 1;
					)";

	int nextUserID = -1;
	executeSqlQuery(selectQuery, getNextUserIDCallback, &nextUserID);

	return nextUserID;
}


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


/**
 @brief		Gets an ID of an album by its name
 @param		albumName		The name of the album to get its ID
 @return	The ID of the specified album
 */
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




// ********************************************************* Get Info In Structure *********************************************************

/**
 @brief		Callback function for a list of the pictures of an album from the PICTURES-table-SELECT-query-response
 @param		data			A pointer to a list of Records where the retrieved pictures will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getAlbumPicturesCallback(void* data, int argc, char** argv, char** azColName)
{
	Record currentPictureRecord;
	
	// Inserting the current picture record into a Record object
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			currentPictureRecord.insert({ "ID", argv[i] });
		}
		else if (std::string(azColName[i]) == "NAME")
		{
			currentPictureRecord.insert({ "NAME", argv[i] });
		}
		else if (std::string(azColName[i]) == "LOCATION")
		{
			currentPictureRecord.insert({ "LOCATION", argv[i] });
		}
		else if (std::string(azColName[i]) == "CREATION_DATE")
		{
			currentPictureRecord.insert({ "CREATION_DATE", argv[i] });
		}
		else if (std::string(azColName[i]) == "ALBUM_ID")
		{
			currentPictureRecord.insert({ "ALBUM_ID", argv[i] });
		}
	}

	static_cast<std::list<Record>*>(data)->push_back(currentPictureRecord);		// Pushing the created Record object into the picture records list

	return 0;
}


/**
 @brief		Returns a list of picture records of the given album
 @param		albumID			The ID of the album to get its pictures
 @return	A list of picture records of the given album
 */
std::list<Record> DatabaseAccess::getAlbumPictures(const int albumID)
{
	std::string getTagsQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM PICTURES
                    WHERE ALBUM_ID = )" + std::to_string(albumID) + R"(;
										
					END TRANSACTION;
					)";

	std::list<Record> picturesList;
	executeSqlQuery(getTagsQuery, getAlbumPicturesCallback, &picturesList);

	return picturesList;
}
