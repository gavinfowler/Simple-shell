//main.cpp
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>

std::vector<std::string> split (const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

    while (getline (ss, item, delim)) {
	        result.push_back (item);
	        }

    return result;
}    
main(void){
	//std::cout << argc <<std::endl;
	std::vector<std::string> history;
	std::chrono::seconds duration;
	std::vector<std::string> arguments;
	/**
	for (int i=0;i<argc;i++){
		std::string temp(argv[i]);
		arguments.push_back(temp);
		std::cout << i <<" : "<<arguments[i]<<std::endl;
	}
	**/
	bool quitting = false;
	std::string input;
	while(!quitting){
		std::cout << "[cmd]: ";
		std::getline (std::cin, input);
		if (input == "exit"){
			quitting = true;
			return 0;
		}

		std::string del = " ";
		char delim = ' ';
		std::string command = input.substr(0, input.find(del));
		std::cout << "command:" << command << std::endl;
		std::string args = input.substr(input.find(del), input.length());
		char *arg = &input[0u];
		arguments = split(arg, delim);
		for (auto const& c : arguments)
    			std::cout << c << ' ';	

                if (input == "ptime")
			quitting = false;
		else if (input == "history")
			quitting = false;
		else if (input[0]=='^'){
			//do stuff here
			std::string delimiter = " ";
			std::string token = input.substr(input.find(delimiter)+1, input.length());
			int temp = 0;
			bool isAnInt = true;
			try {
				temp = std::stoi(token);
			}
			catch(...) {
				std::cout << "Input was not a number.\n";
				isAnInt = false;
			}
			//std::cout << "token:" << token <<std::endl;
			std::cout << "token int:" << temp << std::endl;

		}
		else 
			std::cout << "invalid command\n";
	}
	return 0;
}
