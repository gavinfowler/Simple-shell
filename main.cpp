//main.cpp
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
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
#include <time.h>


void  parse(std::string string, char **argv);
void execute(char **argv, std::vector<std::string> arguments);
int getdir (std::string dir, std::vector<std::string> &files);
std::vector<std::string> split (const std::string &s, char delim);
void signalHandler(int signum __attribute__((unused)));

int main(void){
	//signal(SIGINT, signalHandler);
	/*stuff for living time*/
	struct timeval tv[2]; //need for when started program and when checking difference
	gettimeofday (&tv[0], NULL);
	/*stuff for running time*/
	clock_t t;
	t = clock();
	/*stuff for history and ^ # */
	std::vector<std::string> history;
	int counter = 0;
	/*stuff for ptime*/
	std::chrono::duration<double> dur; 
	std::chrono::seconds duration;
	/*stuff for execvp*/
	std::vector<std::string> arguments;
	/*stuff for general running*/
	bool quitting = false;
	std::string input;
	char pathname[MAXPATHLEN];

	while(!quitting){
/******************JUMPS TO HERE******************************/
retry:
/******************JUMPS TO HERE******************************/

		std::cout << "[" << getcwd(NULL, 0) << "]: ";
		std::getline (std::cin, input);
		counter = 0;
		
/******************JUMPS TO HERE1******************************/
theInput:
/******************JUMPS TO HERE1******************************/

		history.push_back(input);
		char *charinput[input.length()+1];
		parse(input, charinput);
		auto before = std::chrono::high_resolution_clock::now();
		
/******************EXITING******************************/
		if (input == "exit"){
			quitting = true;
			return 0;
		}

		char delim = ' ';
		arguments = split(input, delim);
		/*for (auto a:arguments)
			std::cout << a << " ";
		std::cout <<std::endl;
		*/
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
			std::cout <<"Living time: \n"
				<< std::setw(2) << std::setfill('0') << int(hours) << ":" <<
				std::setw(2) << std::setfill('0')  << int(minutes%60) << ":" <<
				std::setw(2) << std::setfill('0') << int(seconds%60) << "\n";
			quitting = false;
		}
		else if (input == "running_time"){
			int running=clock()-t;
			std::cout << "Running time:\n" 
				<< ((float)running)/CLOCKS_PER_SEC <<std::endl;
		}
		else if (input == "pwd"){
			std::cout << getcwd(NULL, 0) << std::endl;
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
			//std::cout << cwd << std::endl;
			
			std::string dir = std::string(".");
			std::vector<std::string> files = std::vector<std::string>();

			getdir(dir,files);

			for (unsigned int i = 0;i < files.size();i++) {
				std::cout << files[i] << std::endl;
			}
			quitting = false;
		}
		else if (input[0] == 'c' && input[1] =='d' && input[2] ==' '){
			std::string delimiter = " ";
			std::string token = input.substr(input.find(delimiter)+1, input.length());
			/*
			std::string dir = std::string(".");
			std::vector<std::string> files = std::vector<std::string>();
			getdir(dir,files);
			*/
			int ret = chdir(token.c_str());
			//std::cout << getcwd(NULL, 0) << " : " << ret << std::endl;
			if (ret < 0)
				std::cout << "CD ERROR: " << strerror(errno) << std::endl;
			
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
				std::cout << "ERROR: Input was not a number or was formatted incorectly. Format is '^ #' (the space is needed)\n";
				isAnInt = false;
			}
			if(counter>10){
				std::cout << "ERROR: Infinite loop detected\n";
				isAnInt = false;
			}

			if (isAnInt){
				int num = history.size()-temp;
				if (num<=0){
					std::cout << "ERROR: Invalid number choice\n";
					/******JUMP STATEMENT HERE******/
					goto retry;
					/******JUMP STATEMENT HERE******/
				}					
				try{
					input = history[num-1];
				}
				catch(...){
					std::cout << "ERROR: Invalid number choice\n";
					/******JUMP STATEMENT HERE1******/
					goto theInput;
					/******JUMP STATEMENT HERE1******/
				}
				std::cout <<"[cmd]: "<< input << "\n";
				auto after = std::chrono::high_resolution_clock::now();
				dur = after - before;
				/******JUMP STATEMENT HERE1******/
				goto theInput;
				/******JUMP STATEMENT HERE1******/
			}

		}
		else { 
			execute(charinput, arguments);
		}
		auto after = std::chrono::high_resolution_clock::now();
		dur = after - before;
	}
	return 0;
}

void signalHandler(int signum __attribute__((unused))) {
	//std::cout << "\nERROR: Exit with keyword \"exit\"\n" << "[" << getcwd(NULL, 0) << "]: ";
}

void parse(std::string string, char **argv){
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

char *convert(const std::string & s){
	char *pc = new char[s.size()+1];
	std::strcpy(pc, s.c_str());
	return pc; 
}

void execute(char **argv, std::vector<std::string> arguments){
	pid_t pid;
	int status;
	const int READ = 0;
	const int WRITE = 1;
	
	std::string input;
	/*
	for (int i=0; i!=sizeof(argv); i++)
		if(argv[i])
		{
			input.append(std::string(argv[i]) + " ");
			std::cout << std::string(argv[i]);
		}
	std::cout << std::endl;
	
	std::cout << "String: " << input << std::endl;
	input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
	std::cout << "String: " << input << std::endl;
	
	std::string delimiter = "|";
	std::string tokenBefore = input.substr(0,input.find(delimiter));
	std::string tokenAfter = input.substr(input.find(delimiter)+1, input.length());
	std::cout << "String: " << input << std::endl;
	std::cout << "TokenB: " << tokenBefore << std::endl;
	std::cout << "TokenA: " << tokenAfter << std::endl;
	return;
	std::cout << *argv << std::endl;
	*/
	//if (input.find(delimiter) > input.length()){
		/*
		std::string delimiter = " ";
		std::string tokenBefore = input.substr(0,input.find(delimiter));
		std::string tokenAfter = input.substr(input.find(delimiter)+1, input.length());
		const char *cmd = tokenBefore.c_str();
		const char *args[3];
		args[0] = tokenBefore.c_str();
		args[1] = tokenAfter.c_str();
		args[2] = NULL;
		*/
	unsigned int len = std::distance(arguments.begin(), std::find(arguments.begin(), 
				arguments.end(), "|"));
	std::cout << len << ":"<< arguments.size()<<std::endl;
	if(len == arguments.size()){
		if ((pid = fork()) < 0) { 
			std::cout << "ERROR: forking child process failed\n";			
			exit(1);
		}
        	else if (pid == 0) {        
			if (execvp(*argv, argv) < 0) {
				std::cout << strerror(errno) << std::endl;
				exit(1);
			}
		}
		else { 
			wait(&status);
		}
	}
	else{
		std::cout << "piping\n";
		/*
		std::string delimiter = "|";
		std::string tokenBefore = input.substr(0,input.find(delimiter)-1);
		std::string tokenAfter = input.substr(input.find(delimiter)+2, input.length());
		std::cout << "String: " << input << std::endl;
		std::cout << "TokenB: " << tokenBefore << std::endl;
		std::cout << "TokenA: " << tokenAfter << std::endl;

		
		std::string tokenBefore = "";
		std::string tokenAfter = "";
		std::string temporary;
		char* first1[arguments.size()+1];
		char *second1[arguments.size()+1];
		int i=0;
		for(int j=0;j<2;j++){
			i=0;
			temporary="";
			while (true){
				if(arguments.size()==0)
					break;
				std::string temp = arguments[0];
				arguments.erase(arguments.begin());
				if (temp != "|"){
					temporary+=temp;
					if(j==0)
						std::copy(temp.begin(), temp.end(), first1[i]);
					//first1[i]=temp.c_str();
					//else
					//	second1[i]=temp.c_str();
				}
				else
					break;
				temporary+=" ";
				i++;
			}
			if(j==0)
				first1[i]='\0';
			else
				second1[i]='\0';
			if (j==0)
				tokenBefore=temporary;
			else 
				tokenAfter=temporary;
		
		}
		for (int i=0;i<sizeof(first1);i++){
			if(first1[i]=='\0')
				break;
			std::cout << first1[i] <<std::endl;
		}
		for (int i=0;i<sizeof(second1);i++){
			if(second1[i]=='\0')
				break;
			std::cout << second1[i] <<std::endl;
		}

		char *first[tokenBefore.length()+1];	
		parse(tokenBefore, first);
		char *second[tokenAfter.length()+1];
		parse(tokenAfter, second);
		*/
		std::vector<char*> first1;
		std::vector<char*> second1;
		std::transform(arguments.begin(), std::find(arguments.begin(), arguments.end(), "|"),
			       	std::back_inserter(first1), convert);
		first1.push_back('\0');
		second1.push_back('\0');
		std::transform(std::find(arguments.begin(), arguments.end(), "|")+1, arguments.end(),
			       	std::back_inserter(second1), convert);
		for ( size_t i = 0 ; i < second1.size() ; i++ )
			            std::cout << second1[i] << std::endl;
	
		char *first[] = {(char*)"echo", (char*)"hello", (char*)"world", (char*)NULL};
		char *second[] = {(char*)"wc", (char*)NULL};
		int p[2];
		if (pipe(p) !=0) {
			std::cerr << "pipe() failed because: " << strerror(errno) << std::endl;
			return;
		}
		//first
		pid_t cat = fork();
		if (cat == 0) {
			close(p[READ]);
			dup2(p[WRITE], STDOUT_FILENO);
			//execlp((char*)"echo", (char*)"echo", (char*)"hello world", (char*)NULL);
			execvp(first1[0],&first1[0]);
			std::cerr << "Failed to exec1 because " << strerror(errno) << std::endl;
			return;
		}
		//second
		pid_t tr = fork();
    		if (tr == 0) {
	        	close(p[WRITE]);
		        dup2(p[READ], STDIN_FILENO);
			//execlp((char*)"wc", (char*)"wc", (char*)NULL);
			execvp(second1[0], &second1[0]);
			std::cerr << "Failed to exec2 because " << strerror(errno) << std::endl;
			return;
		}
		close(p[READ]);
		close(p[WRITE]);

		std::cerr << "[Parent process]: wait for children to finish...\n";
		int wstatus;
		for (int i = 0; i < 2; ++i) {
			pid_t kiddo = wait(&wstatus);
			
		        if (kiddo == cat) {
				std::cerr << "The first process terminated with status "
				       	<< WEXITSTATUS(wstatus) << std::endl;
			}
			else if (kiddo == tr) {
			std::cerr << "The second process terminated with status " 
				<< WEXITSTATUS(wstatus) << std::endl;
			}
			
		}
		std::cerr << "here1\n";
		first1.clear();
		second1.clear();
		std::cerr << "here2\n";
	}	
	std::cerr << "here3\n";
	return;
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
