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
	signal(SIGINT, signalHandler);
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

		std::cerr << "[" << getcwd(NULL, 0) << "]: ";
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
			std::cout << "here\n";
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

struct command{
	  const char **argv;
};

int spawn_proc (int in, int out, std::vector<char*> cmd){
	pid_t pid;
	if ((pid = fork ()) == 0){
		if (in != 0){
			dup2 (in, 0);
			close (in);
		}
	        if (out != 1){
			dup2 (out, 1);
			close (out);
		}
	        if(execvp (cmd[0], &cmd[0])<0){
			std::cerr << strerror(errno)<<std::endl;
			exit(1);
		}
		return -1;
        }
	return pid;
}

int fork_pipes (int n, std::vector<std::vector<char*>> cmd){
	int i;
	//pid_t pid;
	int in, fd [2];
        /* The first process should get its input from the original file descriptor 0.  */
        in = 0;
        /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
      	for (i = 0; i < n - 1; ++i){
		pipe (fd);
                /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
		std::cerr << i << std::endl;
                spawn_proc (in, fd [1], cmd[i]);
 	        /* No need for the write end of the pipe, the child will write here.  */
	        close (fd [1]);
                /* Keep the read end of the pipe, the next child will read from there.  */
	        in = fd [0];
	}
        /* Last stage of the pipeline - set stdin be the read end of the previous pipe
	 *      and output to the original file descriptor 1. */  
	if (in != 0)
		dup2 (in, 0);
	/* Execute the last stage with the current process. */
	std::vector<char*> argv = cmd[i];
	std::cerr << i << std::endl;
	if (execvp (argv[0], &argv[0])<0){
		std::cerr << strerror(errno)<<std::endl;
		exit(1);
	}
	return 0;
}


void execute(char **argv, std::vector<std::string> arguments){
	pid_t pid;
	int status;
	const int READ = 0;
	const int WRITE = 1;	
	unsigned int len = std::distance(arguments.begin(), std::find(arguments.begin(), 
				arguments.end(), "|"));
	std::cout << "len: " << len << std::endl;
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
		std::vector<std::vector<char*>> cmd;
		//struct command cmd [arguments.size()+1];
		std::vector<char*> first1;
		int i = 0;
		std::cout << "prewhileloop\n";
		std::cout << std::distance(arguments.begin(), std::find(arguments.begin(), 
				arguments.end(), "|"))<<std::endl;
		while(true){
			first1.clear();
			std::transform(arguments.begin(), std::find(arguments.begin(),
				arguments.end(), "|"), std::back_inserter(first1), convert);
			std::cout << "size: " << arguments.size() <<std::endl;
			first1.push_back((char*)NULL);
			cmd.push_back(first1);
			std::cout << "i: "<< i <<std::endl;
			i++;
			arguments.erase(arguments.begin(),
				std::find(arguments.begin(), arguments.end(), "|"));
			std::cout << "distance: " << std::distance(arguments.begin(), 
				std::find(arguments.begin(), arguments.end(), "|")) << std::endl;
			if (std::distance(arguments.begin(), std::find(arguments.begin(), 
				arguments.end(), "|")) == 0){
				if(arguments.size() != 0)
					arguments.erase(arguments.begin());
				else
					break;
			}
			//std::cout << first1.size() <<std::endl;
		}
		std::cout << "prepiping\n" << "size: " << cmd.size()<<std::endl;
		for (auto a:cmd)
			std::cerr << a[0] << std::endl;
		fork_pipes(cmd.size(), cmd);
		return;
		

		//std::vector<char*> first1;
		std::vector<char*> second1;
		std::transform(arguments.begin(), std::find(arguments.begin(), arguments.end(), "|"),
			       	std::back_inserter(first1), convert);
		first1.push_back('\0');
		std::transform(std::find(arguments.begin(), arguments.end(), "|")+1, arguments.end(),
			       	std::back_inserter(second1), convert);
		second1.push_back('\0');
	
		//char *first[] = {(char*)"echo", (char*)"hello", (char*)"world", (char*)NULL};
		//char *second[] = {(char*)"wc", (char*)NULL};
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

		//std::cerr << "[Parent process]: wait for children to finish...\n";
		int wstatus;
		for (int i = 0; i < 2; ++i) {
			//pid_t kiddo = wait(&wstatus);
			wait(&wstatus);
			/*
		        if (kiddo == cat) {
				std::cerr << "The first process terminated with status "
				    	<< WEXITSTATUS(wstatus) << std::endl;
			}
			else if (kiddo == tr) {
				std::cerr << "The second process terminated with status " 
					<< WEXITSTATUS(wstatus) << std::endl;
			}
			*/
		}
	}	
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
