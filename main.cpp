//main.cpp
#include <string>
#include <string.h>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>



void  parse(std::string string, char **argv)
{
	char *line = new char[string.length()+1];
	//std::cout << "here\n";
	strcpy(line, string.c_str());
	//std::cout << "here2\n";
	while (*line != '\0') {
		while (*line == ' ' || *line == '\t' || *line == '\n')
        		*line++ = '\0';
		*argv++ = line;
		while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n')
			line++;
	}
        *argv = '\0'; 
	//std::cout << "here3\n";
	//delete [] line;
	//line = NULL;
}

	
void execute(char **argv)
{
	pid_t pid;
	int status;

	if ((pid = fork()) < 0) { 
		printf("*** ERROR: forking child process failed\n");				
		exit(1);
	}
        else if (pid == 0) {        
		if (execvp(*argv, argv) < 0) {
			std::cout << argv <<std::endl;
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
	}
	else { 
		while (wait(&status) != pid)
			;

	}
}

int getdir (std::string dir, std::vector<std::string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL) {
		std::cout << "Error(" << errno << ") opening " << dir << std::endl;
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
		char *charinput[input.length()+1];
		//std::cout << "preparse\n";
		parse(input, charinput);
		//std::cout << "postparse\n";
		auto before = std::chrono::high_resolution_clock::now();
		if (input == "exit"){
			quitting = true;
			return 0;
		}

		char delim = ' ';
		/*
		std::string command = input.substr(0, input.find(del));
		std::cout << "command:" << command << std::endl;
		std::string args = input.substr(input.find(del), input.length());
		std::cout << "here\n";
		char *arg = &input[0u];
		*/
		arguments = split(input, delim);
		/*
		for (auto const& c : arguments)
    			std::cout << c << ' ';
		std::cout << std::endl;	
		std::cout << "command:"<<arguments[0]<<std::endl;
		*/
                if (input == "ptime"){
			std::cout << "Time spent executing child processes: "<< dur.count() <<" seconds\n";
			quitting = false;
		}
		else if (input == "history"){
			std::cout << std::endl;
			for (unsigned int i=1;i<=history.size();i++)
				std::cout << i << " : " << history[i] << std::endl;
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
		else { 
			//std::cout <<charinput<<std::endl;
			execute(charinput);
			//std::cout << "invalid command\n";
		}
		auto after = std::chrono::high_resolution_clock::now();
		dur = after - before;
		history.push_back(input);
	}
	return 0;
}
