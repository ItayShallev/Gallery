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


DatabaseAccess::~DatabaseAccess()
{
	sqlite3_close(this->_DB);
}


/**
 @brief			Initializes the Gallery database by creating necessary tables if they do not exist.
 @return		True if the database is successfully initialized, false otherwise
 */
bool DatabaseAccess::initDatabase()
{
	std::string statement = std::string(USERS_TABLE_SQL_STATEMENT) + ALBUMS_TABLE_SQL_STATEMENT + PICTURES_TABLE_SQL_STATEMENT + TAGS_TABLE_SQL_STATEMENT;
	char* errMessage = nullptr;

	int res = sqlite3_exec(this->_DB, statement.c_str(), nullptr, nullptr, &errMessage);

	if (res != SQLITE_OK)
	{
		this->_DB = nullptr;
		std::cout << "Failed to send the query: " << std::endl << errMessage << std::endl;

		return false;
	}

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
