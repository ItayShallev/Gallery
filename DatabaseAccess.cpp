#include "DatabaseAccess.h"


DatabaseAccess::DatabaseAccess() : _DB(nullptr) { };


DatabaseAccess::~DatabaseAccess()
{
	sqlite3_close(this->_DB);
}




// ********************************************************* Album *********************************************************

/**
 @brief		Returns all the albums in the database
 @return	A list of all the albums in the database (List<Album(Pictures(Tags))>)
 */
const std::list<Album> DatabaseAccess::getAlbums()
{
	return this->buildAlbumsList(this->getAlbumsRecords());
}


/**
 @brief		Returns all the albums of the given user
 @param		user	The user to get its albums
 @return	A list of all the albums of the given user (List<Album(Pictures(Tags))>)
 */
const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	return this->buildAlbumsList(this->getUserAlbumsRecords(user.getId()));
}


/**
 @brief		Creates a new album in the Albums table in the database
 @param     album		The album to create in the Albums table
 @return	Void
 */
void DatabaseAccess::createAlbum(const Album& album)
{
	std::string createAlbumStatement = R"(
					INSERT INTO ALBUMS
					(NAME, CREATION_DATE, USER_ID)
					VALUES (')" + album.getName() + "', '" + album.getCreationDate() + "', " + std::to_string(album.getOwnerId()) + R"()
					)";

	executeSqlStatement(createAlbumStatement);
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
	std::list<Record> albumPictures = this->getAlbumPicturesRecords(albumID);

	// Iterating over the pictures list, and removing all album inner data (Picture-Tags and Pictures) before removing the album itself
	for (auto picturesIterator = albumPictures.begin(); picturesIterator != albumPictures.end(); ++picturesIterator)
	{
		this->removePictureTags(std::stoi(picturesIterator->at("ID")));
		this->removePicture(std::stoi(picturesIterator->at("ID")));
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


/**
 @brief		Callback function for checking the result-rows-count to figure if a given album exists
 @param		data			A pointer to a boolean variable where the answer to the "question" will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::doesAlbumExistsCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<bool*>(data)) = std::stoi(argv[0]) != 0;

	return 0;
}


/**
 @brief		Checks if a given album exists on the database
 @param		albumName		The name of the album to check if exists on the database
 @param		userId			The user that owns the album to check if exists on the database
 @return	True if the album exists, false otherwise
 */
bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::string doesAlbumExistsQuery = R"(
					BEGIN TRANSACTION;
					
					SELECT COUNT(*) FROM ALBUMS
					WHERE NAME = ')" + albumName + "' AND USER_ID = " + std::to_string(userId) + R"(;
					
					END TRANSACTION;
					)";

	bool doesAlbumExist = false;
	executeSqlQuery(doesAlbumExistsQuery, doesAlbumExistsCallback, &doesAlbumExist);

	return doesAlbumExist;
}


/**
 @brief		Returns an Album object of the album with the given name from the database
 @param     albumName        The name of the album to return as an Album object
 @return	An Album object of the album with the given name from the database
 */
Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	return this->buildAlbumsList(this->getAlbumRecordsByName(albumName)).front();
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


/**
 @brief		Prints all the albums in the database
 @return	Void
 */
void DatabaseAccess::printAlbums()
{
	std::list<Record> albumsRecords = this->getAlbumsRecords();

	std::cout << "Album list:" << std::endl;
	std::cout << "-----------" << std::endl;

	// Iterating over the albums records and printing each album
	for (auto albumsIterator = albumsRecords.begin(); albumsIterator != albumsRecords.end(); ++albumsIterator)
	{
		Album currentAlbum(std::stoi(albumsIterator->at("USER_ID")), albumsIterator->at("NAME"), albumsIterator->at("CREATION_DATE"));

		std::cout << std::setw(5) << "* " << currentAlbum;
	}
}




// ********************************************************* Picture *********************************************************

/**
 @brief		Adds a new picture to an album in the database
 @param     albumName		The name of the album to add the picture to
 @param     picture			The Picture object to add to the album
 @return	Void
 */
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	int albumID = this->getAlbumID(albumName);

	std::string addPictureStatement = R"(
					BEGIN TRANSACTION;
					
					INSERT INTO PICTURES
					(ID, NAME, LOCATION, CREATION_DATE, ALBUM_ID)
					VALUES ()" + std::to_string(picture.getId()) + ", '" + picture.getName() + "' , '" + picture.getPath() + "', '" +
													picture.getCreationDate() + "', " + std::to_string(albumID) + R"();
					
					END TRANSACTION;
					)";

	executeSqlStatement(addPictureStatement);
}

/**
 @brief     Removes a picture from an album in the database
 @param     albumName       The name of the album to remove the picture from
 @param     pictureName     The name of the picture to remove from the album
 @return    Void
 */
void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	removePicture(this->getPictureID(pictureName, this->getAlbumID(albumName)));
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

/**
 @brief		Prints all the users in the database
 @return	Void
 */
void DatabaseAccess::printUsers()
{
	std::list<Record> usersRecords = this->getUsersRecords();

	std::cout << "Users list:" << std::endl;
	std::cout << "-----------" << std::endl;

	// Iterating over the users records and printing each user
	for (auto usersIterator = usersRecords.begin(); usersIterator != usersRecords.end(); ++usersIterator)
	{
		User currentUser(std::stoi(usersIterator->at("ID")), usersIterator->at("NAME"));

		std::cout << currentUser << std::endl;
	}
}


/**
 @brief		Returns a User object of the user with the given id from the database
 @param     userId          The id of the user to return as a User object
 @return	A User object of the user with the given id from the database
 */
User DatabaseAccess::getUser(int userId)
{
	return User(userId, this->getUserName(userId));
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


/**
 @brief		Callback function for checking the result-rows-count to figure if a given user exists
 @param		data			A pointer to a boolean variable where the answer to the "question" will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::doesUserExistsCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<bool*>(data)) = std::stoi(argv[0]) != 0;

	return 0;
}


/**
 @brief		Checks if a given user exists on the database
 @param		userId			The user of the user to check if exists on the database
 @return	True if the user exists, false otherwise
 */
bool DatabaseAccess::doesUserExists(int userId)
{
	std::string doesUserExistsQuery = R"(
					BEGIN TRANSACTION;
					
					SELECT COUNT(*) FROM ALBUMS
					WHERE USER_ID = )" + std::to_string(userId) + R"(;
					
					END TRANSACTION;
					)";

	bool doesUserExist = false;
	executeSqlQuery(doesUserExistsQuery, doesUserExistsCallback, &doesUserExist);

	return doesUserExist;
}




// ********************************************************* Users Statistics *********************************************************

/**
 @brief		Callback function for getting the amount of albums that a user owns
 @param		data			A pointer to an integer where the retrieved number of albums will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return
 */
int DatabaseAccess::countAlbumsOwnedOfUserCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<int*>(data)) = std::stoi(argv[0]);

	return 0;
}


/**
 @brief		Returns the number of albums owned by a given user
 @param     userId          The ID of the user to count how many albums he ownes
 @return	The number of albums owned by the given user
 */
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::string countUserAlbumsQuery = R"(
					BEGIN TRANSACTION;
                    
                    SELECT COUNT(*) FROM ALBUMS
					WHERE USER_ID = )" + std::to_string(user.getId()) + R"(;
					
					END TRANSACTION;					
					)";

	int userAlbumsNumber = 0;
	if (!executeSqlQuery(countUserAlbumsQuery, countAlbumsOwnedOfUserCallback, &userAlbumsNumber)) { return -1; };

	return userAlbumsNumber;
}


/**
 @brief		Counts the amount of different albums that a given user is tagged in
 @param     user		The user to count how many different albums he is tagged in
 @return	The amount of different albums that the given user is tagged in
 */
int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	int userAlbumsTagsCounter = 0;

	std::list<Album> albums = this->getAlbums();

	// Iterating over the DB albums
	for (auto albumsIterator = albums.begin(); albumsIterator != albums.end(); ++albumsIterator)
	{
		std::list<Picture> albumPictures = albumsIterator->getPictures();

		// Iterating over the album pictures and checking if the given user is tagged in one of them
		for (auto picturesIterator = albumPictures.begin(); picturesIterator != albumPictures.end(); ++picturesIterator)
		{
			if (picturesIterator->isUserTagged(user))
			{
				userAlbumsTagsCounter++;
				break;
			}
		}
	}

	return userAlbumsTagsCounter;
}


/**
 @brief		Counts the amount of time a given user is tagged in any picture in the database
 @param     user		The user to count how many times he is tagged in any picture
 @return	The amount of times a given user is tagged
 */
int DatabaseAccess::countTagsOfUser(const User& user)
{
	int userTagsCounter = 0;

	std::list<Album> albums = this->getAlbums();

	// Iterating over the DB albums
	for (auto albumsIterator = albums.begin(); albumsIterator != albums.end(); ++albumsIterator)
	{
		std::list<Picture> albumPictures = albumsIterator->getPictures();

		// Iterating over the album pictures and counting how many times the given user is tagged
		for (auto picturesIterator = albumPictures.begin(); picturesIterator != albumPictures.end(); ++picturesIterator)
		{
			if (picturesIterator->isUserTagged(user))
			{
				userTagsCounter++;
			}
		}
	}

	return userTagsCounter;
}


/**
 @brief		Returns the average tags per album of the given user
 @param     user        The user get the average tags per album of
 @return	The average tags per album of the given user
 */
float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = this->countAlbumsTaggedOfUser(user);

	if (albumsTaggedCount == 0) { return 0; }

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}




// ********************************************************* Queries *********************************************************

/**
 @brief		Callback function for getting the top tagged user in the database
 @param		data			A pointer to a User object where the top tagged user will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getTopTaggedUserCallback(void* data, int argc, char** argv, char** azColName)
{
	User* topTaggedUser = static_cast<User*>(data);

	// Iterating over the columns of the result set, building the User object
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "USER_ID")
		{
			topTaggedUser->setId(std::stoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME")
		{
			topTaggedUser->setName(argv[i]);
		}
	}

	return 0;
}


/**
 @brief		Returns the top tagged user in the database
 @return	The top tagged user in the database
 */
User DatabaseAccess::getTopTaggedUser()
{
	std::string getTopTaggedUserQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT TAGS.USER_ID, USERS.NAME
		            FROM TAGS INNER JOIN USERS ON TAGS.USER_ID = USERS.ID
		            GROUP BY TAGS.USER_ID
		            ORDER BY COUNT(TAGS.USER_ID) DESC
	                LIMIT 1;
					
					END TRANSACTION;
					)";

	User topTaggedUser(-1, "");
	executeSqlQuery(getTopTaggedUserQuery, getTopTaggedUserCallback, &topTaggedUser);

	return topTaggedUser;
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
 @brief			Does nothing, nothing to clean
 @return		Void
 @note			Needed to implemented since its a pure virtual function in the base class
 */
void DatabaseAccess::clear()
{

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
 @brief		Callback function for getting the next ID before a new object creation in the database
 @param		data			A pointer to an integer where the retrieved ID will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getNextIDCallback(void* data, int argc, char** argv, char** azColName)
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
					BEGIN TRANSACTION;

					SELECT ID FROM USERS
	                ORDER BY ID DESC
					LIMIT 1;

                    END TRANSACTION;
					)";

	int nextUserID = -1;
	executeSqlQuery(selectQuery, getNextIDCallback, &nextUserID);

	return nextUserID;
}


/**
 @brief		Returns the next picture ID to create for a new picture
 @return	The next picture ID to create for a new picture
 */
int DatabaseAccess::getNextPictureID()
{
	std::string selectQuery = R"(
					BEGIN TRANSACTION;

					SELECT ID FROM PICTURES
	                ORDER BY ID DESC
					LIMIT 1;

                    END TRANSACTION;
					)";

	int nextPictureID = -1;
	executeSqlQuery(selectQuery, getNextIDCallback, &nextPictureID);

	return nextPictureID;
}


/**
 @brief		Callback function for getting the name of a user from a USERS-table-select-query-response
 @param     data            A pointer to a string where the retrieved name will be stored
 @param     argc            The number of columns in the result set
 @param     argv            An array of strings representing the result set
 @param     azColName       An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getUserNameCallback(void* data, int argc, char** argv, char** azColName)
{
	*(static_cast<std::string*>(data)) = argv[0];

	return 0;
}


/**
 @brief		Returns the name of the user with the given ID
 @param     userId        The ID of the user to retrieve its name
 @return	The name of the user with the given ID
 */
std::string DatabaseAccess::getUserName(const int userID)
{
	std::string getUserNameQuery = R"(
					BEGIN TRANSACTION;

					SELECT NAME FROM USERS
					WHERE ID = )" + std::to_string(userID) + R"(;

                    END TRANSACTION;
					)";

	std::string userName = "";
	executeSqlQuery(getUserNameQuery, getUserNameCallback, &userName);

	return userName;
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
 @brief		Callback function for getting a list of all albums from the ALBUMS-table-SELECT-query-response
 @param		data			A pointer to a list of Records where the retrieved albums will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getAlbumsRecordsCallback(void* data, int argc, char** argv, char** azColName)
{
	Record currentAlbumRecord;

	// Inserting the current album record into a Record object
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			currentAlbumRecord.insert({ "ID", argv[i] });
		}
		else if (std::string(azColName[i]) == "NAME")
		{
			currentAlbumRecord.insert({ "NAME", argv[i] });
		}
		else if (std::string(azColName[i]) == "CREATION_DATE")
		{
			currentAlbumRecord.insert({ "CREATION_DATE", argv[i] });
		}
		else if (std::string(azColName[i]) == "USER_ID")
		{
			currentAlbumRecord.insert({ "USER_ID", argv[i] });
		}
	}

	static_cast<std::list<Record>*>(data)->push_back(currentAlbumRecord);		// Pushing the created Record object into the albums records list

	return 0;
}


/**
 @brief		Returns a list of all album records from the database
 @return	A list of all the album records in the database
 */
std::list<Record> DatabaseAccess::getAlbumsRecords()
{
	std::string getAlbumsQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM ALBUMS;
										
					END TRANSACTION;
					)";

	std::list<Record> albumsList;
	executeSqlQuery(getAlbumsQuery, getAlbumsRecordsCallback, &albumsList);

	return albumsList;
}


/**
 @brief		Returns a list of all the album records of the given user from the database
 @param     userID			The ID of the user to get the album records of
 @return	A list of all the album records of the given user in the database
 */
std::list<Record> DatabaseAccess::getUserAlbumsRecords(const int userID)
{
	std::string getUserAlbumsQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM ALBUMS
					WHERE USER_ID = )" + std::to_string(userID) + R"(;
										
					END TRANSACTION;
					)";

	std::list<Record> userAlbumsList;
	executeSqlQuery(getUserAlbumsQuery, getAlbumsRecordsCallback, &userAlbumsList);

	return userAlbumsList;
}


/**
 @brief		Returns a list of a single album record with the given name from the database
 @param     albumName		The name of the album to get the record of
 @return	list of a single album record with the given name from the database
 */
std::list<Record> DatabaseAccess::getAlbumRecordsByName(const std::string albumName)
{
	std::string getUserAlbumsQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM ALBUMS
					WHERE NAME = ')" + albumName + R"(';
										
					END TRANSACTION;
					)";

	std::list<Record> userAlbumsList;
	executeSqlQuery(getUserAlbumsQuery, getAlbumsRecordsCallback, &userAlbumsList);

	return userAlbumsList;
}


/**
 @brief		Builds a list of Album objects from the given list of album records
 @param     albumsList        The list of album records to build the Album objects from
 @return	A list of Album objects built from the given list of album records
 */
std::list<Album> DatabaseAccess::buildAlbumsList(const std::list<Record>& albumsRecords)
{
	std::list<Album> albumsList;

	// Adding all the albums in the DB to the Albums list
	for (auto albumsIterator = albumsRecords.begin(); albumsIterator != albumsRecords.end(); ++albumsIterator)
	{
		Album currentAlbum(std::stoi(albumsIterator->at("USER_ID")), albumsIterator->at("NAME"), albumsIterator->at("CREATION_DATE"));

		std::list<Record> albumPicturesRecords = this->getAlbumPicturesRecords(std::stoi(albumsIterator->at("ID")));

		// Adding all the pictures in the album to the Album object
		for (auto picturesIterator = albumPicturesRecords.begin(); picturesIterator != albumPicturesRecords.end(); ++picturesIterator)
		{
			Picture currentPicture(std::stoi(picturesIterator->at("ID")), picturesIterator->at("NAME"), picturesIterator->at("LOCATION"), picturesIterator->at("CREATION_DATE"));

			std::list<Record> pictureTagsRecords = this->getPictureTagsRecords(currentPicture.getId());

			// Adding all the tags in the picture to the Picture object
			for (auto tagsIterator = pictureTagsRecords.begin(); tagsIterator != pictureTagsRecords.end(); ++tagsIterator)
			{
				currentPicture.tagUser(std::stoi(tagsIterator->at("USER_ID")));		// Adding the current tag to the Picture object
			}

			currentAlbum.addPicture(currentPicture);		// Adding the current picture to the Album object
		}

		albumsList.push_back(currentAlbum);		// Adding the current album to the Albums list
	}

	return albumsList;
}


/**
 @brief		Callback function for getting a list of the pictures of an album from the PICTURES-table-SELECT-query-response
 @param		data			A pointer to a list of Records where the retrieved pictures will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getAlbumPicturesRecordsCallback(void* data, int argc, char** argv, char** azColName)
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

	static_cast<std::list<Record>*>(data)->push_back(currentPictureRecord);		// Pushing the created Record object into the pictures records list

	return 0;
}


/**
 @brief		Returns a list of picture records of the given album
 @param		albumID			The ID of the album to get its pictures
 @return	A list of picture records of the given album
 */
std::list<Record> DatabaseAccess::getAlbumPicturesRecords(const int albumID)
{
	std::string getAlbumPicturesQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM PICTURES
                    WHERE ALBUM_ID = )" + std::to_string(albumID) + R"(;
										
					END TRANSACTION;
					)";

	std::list<Record> picturesList;
	executeSqlQuery(getAlbumPicturesQuery, getAlbumPicturesRecordsCallback, &picturesList);

	return picturesList;
}


/**
 @brief		Callback function for getting a list of the tags of a picture from the TAGS-table-SELECT-query-response
 @param		data			A pointer to a list of Records where the retrieved tags will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getPictureTagsRecordsCallback(void* data, int argc, char** argv, char** azColName)
{
	Record currentTagRecord;

	// Inserting the current tag record into a Record object
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			currentTagRecord.insert({ "ID", argv[i] });
		}
		else if (std::string(azColName[i]) == "PICTURE_ID")
		{
			currentTagRecord.insert({ "PICTURE_ID", argv[i] });
		}
		else if (std::string(azColName[i]) == "USER_ID")
		{
			currentTagRecord.insert({ "USER_ID", argv[i] });
		}
	}

	static_cast<std::list<Record>*>(data)->push_back(currentTagRecord);		// Pushing the created Record object into the tags records list

	return 0;
}


/**
 @brief		Returns a list of tag records of the given picture
 @param		pictureID			The ID of the picture to get its tags
 @return	A list of tag records of the given picture
 */
std::list<Record> DatabaseAccess::getPictureTagsRecords(const int pictureID)
{
	std::string getPictureTagsQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM TAGS
                    WHERE PICTURE_ID = )" + std::to_string(pictureID) + R"(;
										
					END TRANSACTION;
					)";

	std::list<Record> tagsList;
	executeSqlQuery(getPictureTagsQuery, getPictureTagsRecordsCallback, &tagsList);

	return tagsList;
}


/**
 @brief		Callback function for getting a list of all users in the database from the USERS-table-SELECT-query-response
 @param		data			A pointer to a list of Records where the retrieved users will be stored
 @param		argc			The number of columns in the result set
 @param		argv			An array of strings representing the result set
 @param		azColName		An array of strings containing the column names of the result set
 @return	Always returns 0
 */
int DatabaseAccess::getUsersRecordsCallback(void* data, int argc, char** argv, char** azColName)
{
	Record currentUserRecord;

	// Inserting the current user record into a Record object
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			currentUserRecord.insert({ "ID", argv[i] });
		}
		else if (std::string(azColName[i]) == "NAME")
		{
			currentUserRecord.insert({ "NAME", argv[i] });
		}
	}

	static_cast<std::list<Record>*>(data)->push_back(currentUserRecord);		// Pushing the created Record object into the users records list

	return 0;
}


/**
 @brief		Returns a list of all user records in the database
 @return	A list of all user records in the database
 */
std::list<Record> DatabaseAccess::getUsersRecords()
{
	std::string getUsersQuery = R"(
					BEGIN TRANSACTION;
					
                    SELECT * FROM USERS;
										
					END TRANSACTION;
					)";

	std::list<Record> usersList;
	executeSqlQuery(getUsersQuery, getUsersRecordsCallback, &usersList);

	return usersList;
}
