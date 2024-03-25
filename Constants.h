#pragma once
#include <string>
#include <vector>
#include <map>

const std::string DB_FILE_NAME = "Gallery.sqlite";

enum CommandType
{
	HELP = 0,
	// Album operations
	CREATE_ALBUM = 1,
	OPEN_ALBUM,
	CLOSE_ALBUM,
	DELETE_ALBUM,
	LIST_ALBUMS,
	LIST_ALBUMS_OF_USER,

	// Picture operations
	ADD_PICTURE,
	REMOVE_PICTURE,
	SHOW_PICTURE,
	LIST_PICTURES,
	TAG_USER,
	UNTAG_USER,
	LIST_TAGS,

	// User operations
	ADD_USER,
	REMOVE_USER,
	LIST_OF_USER,
	USER_STATISTICS,
	

	TOP_TAGGED_USER,
	TOP_TAGGED_PICTURE,
	PICTURES_TAGGED_USER,

	EXIT = 99
};

struct CommandPrompt {
	CommandType type;
	const std::string prompt;
};

struct CommandGroup {
	const std::string title;
	const std::vector<struct CommandPrompt> commands;
};

typedef std::map<std::string, std::string> Record;

#define DATABASE_EXISTS 0


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
#define INSERT_TO_USERS R"(INSERT INTO USERS (NAME) VALUES ('User1'), ('User2'), ('User3'), ('User4'), ('User5'),  ('User6');)"

#define INSERT_TO_ALBUMS R"(INSERT INTO ALBUMS
(NAME, CREATION_DATE, USER_ID)
VALUES ('Album1', '01/01/2001', 1), ('Album2', '02/02/2002', 2), ('Album3', '03/03/2003', 3),
('Album4', '04/04/2004', 4), ('Album5', '05/05/2005', 5), ('Album6', '06/06/2006', 6), ('Album7', '07/06/2006', 5);)"

#define INSERT_INTO_PICTURES R"(INSERT INTO PICTURES
(NAME, LOCATION, CREATION_DATE, ALBUM_ID)
VALUES ('Picture1', 'C:\Users\Itay\Downloads\Picture1.png', '01/01/2001', 1),
('Picture2.png', 'C:\Users\Itay\Downloads\Picture2.png', '02/02/2002', 2),
('Picture3.png', 'C:\Users\Itay\Downloads\Picture3.png', '03/03/2003', 3),
('Picture4.png', 'C:\Users\Itay\Downloads\Picture4.png', '04/04/2004', 4),
('Picture5.png', 'C:\Users\Itay\Downloads\Picture5.png', '05/05/2005', 5),
('Picture6.png', 'C:\Users\Itay\Downloads\Picture6.png', '06/06/2006', 6);)"

#define INSERT_TO_TAGS R"(INSERT INTO TAGS
(PICTURE_ID, USER_ID)
VALUES (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6);)"
// DEBUG
