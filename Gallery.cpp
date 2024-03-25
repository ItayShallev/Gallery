#include <iostream>
#include <string>
#include "MemoryAccess.h"
#include "AlbumManager.h"
#include "DatabaseAccess.h"

#define DATE_TIME_BUFFER_SIZE	100


int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}


void printDateAndTime()
{
	std::time_t currentTime = std::time(nullptr);
	char timeBuffer[DATE_TIME_BUFFER_SIZE];

	std::strftime(timeBuffer, 80, "%d-%m-%Y %H:%M", std::localtime(&currentTime));
	std::cout << "Current date and time: " << timeBuffer << std::endl;
}


int main(void)
{
	// initialization data access
	DatabaseAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);

	std::string albumName;
	std::cout << "Welcome to Gallery, developed by Itay Shalev!" << std::endl;

	printDateAndTime();
	std::cout << "=============================================" << std::endl << std::endl;

	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;


	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}
