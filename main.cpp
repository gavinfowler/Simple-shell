//main.cpp
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


void  parse(std::string string, char **argv){
	char *line = new char[string.length()+1];
	strcpy(line, string.c_str());
	while (*line != '\0') {
		while (*line == ' ' || *line == '\t' || *line == '\n')
        		*line++ = '\0';
		*argv++ = line;
		while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n')
			line++;
	}
        *argv = '\0'; 
}

	
void execute(char **argv){
	pid_t pid;
	int status;

	if ((pid = fork()) < 0) { 
		std::cout << "*** ERROR: forking child process failed\n";			
		exit(1);
	}
        else if (pid == 0) {        
		if (execvp(*argv, argv) < 0) {
			std::cout <<"ERROR: Invalid Input, Command not recognized\n";
			exit(1);
		}
	}
	else { 
		while (wait(&status) != pid)
			;

	}
}

int getdir (std::string dir, std::vector<std::string> &files){
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL) {
		std::cout << "ERROR: " << strerror(errno) << ", attemped to open " 
			<< dir << std::endl;
		return errno;
	}
        while ((dirp = readdir(dp)) != NULL) {
        	files.push_back(std::string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

char pathname[MAXPATHLEN];
std::chrono::duration<double> dur; 

std::vector<std::string> split (const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	try{
    		while (getline (ss, item, delim)) {
	        	result.push_back (item);
	        }
	}
	catch(...){
		result.push_back(s);
	}
    	return result;
}    

int main(void){
	struct timeval tv[2]; //need 2 for when started program and when checking difference
	gettimeofday (&tv[0], NULL);
	std::vector<std::string> history;
	std::chrono::seconds duration;
	std::vector<std::string> arguments;
	int counter = 0;
	bool quitting = false;
	std::string input;
	while(!quitting){
		std::cout << "[cmd]: ";
		std::getline (std::cin, input);
		counter = 0;
/******************JUMPS TO HERE******************************/
theInput:
/******************JUMPS TO HERE******************************/
		history.push_back(input);
		char *charinput[input.length()+1];
		parse(input, charinput);
		auto before = std::chrono::high_resolution_clock::now();
		if (input == "exit"){
			quitting = true;
			return 0;
		}

		char delim = ' ';
		arguments = split(input, delim);
                if (input == "ptime"){
			std::cout << "Time spent executing child processes: "<< dur.count() 
				<<" seconds\n";
			quitting = false;
		}
		else if (input == "living_time"){
			gettimeofday (&tv[1], NULL);
			int diff = ((tv[1].tv_sec - tv[0].tv_sec));
			int seconds = diff;
			int minutes = seconds / 60;
			int hours = minutes / 60;
			std::cout << std::setw(2) << std::setfill('0') << int(hours) << ":" <<
				std::setw(2) << std::setfill('0')  << int(minutes%60) << ":" <<
				std::setw(2) << std::setfill('0') << int(seconds%60) << "\n";
			quitting = false;
		}
		else if (input == "history"){
			std::cout << std::endl;
			for (unsigned int i=1;i<=history.size();i++)
				std::cout << i << " : " << history[i-1] << std::endl;
			std::cout << std::endl;
			quitting = false;
		}
		else if (input == "ls"){
			std::string cwd = getcwd(pathname, MAXPATHLEN);
			std::cout << cwd << std::endl;
			
			std::string dir = std::string(".");
			std::vector<std::string> files = std::vector<std::string>();

			getdir(dir,files);

			for (unsigned int i = 0;i < files.size();i++) {
				std::cout << files[i] << std::endl;
			}
			quitting = false;
		}
		else if (input[0]=='^'){
			counter++;
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
			if(counter>10){
				std::cout << "ERROR: Infinite loop detected\n";
				isAnInt = false;
			}

			if (isAnInt){
				int num = history.size()-temp;
				try{
					input = history[num-1];
				}
				catch(...){
					std::cout << "ERROR: Invalid number choice\n";
					/******JUMP STATEMENT******/
					goto theInput;
					/******JUMP STATEMENT******/
				}
				std::cout <<"[cmd]: "<< input << "\n";
				auto after = std::chrono::high_resolution_clock::now();
				dur = after - before;
				/******JUMP STATEMENT******/
				goto theInput;
				/******JUMP STATEMENT******/
			}

		}
		else { 
			execute(charinput);
		}
		auto after = std::chrono::high_resolution_clock::now();
		dur = after - before;
	}
	return 0;
}
