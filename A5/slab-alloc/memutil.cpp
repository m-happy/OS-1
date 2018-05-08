#include <iostream>
#include <cstring>
#include <thread>
#include <ctime>
#include <cstdlib>		
#include <unistd.h>     /* usleep() */
#include "libmymem.hpp"


void threadFunc(int n){
	for(int i=0; i<n; i++){
		unsigned size = (rand()%8192)+1;
		void * ptr = mymalloc(size);
		memset(ptr, 0, size);
		usleep(rand()%300+ 100);
		myfree(ptr);
	}
	return ;
} // threadFunc

int main(int argv, char**argc){
	int n, t;
	// Number of iterations and number of threads.
	srand(time(NULL)); // seed random number generator.
	
	// Argument parsing
	if(argv!=5) {std::cerr<<"Pls provide arguments"<<std::endl; return 0;}
	else{
		if(strcmp("-n", argc[1])==0 && strcmp("-t", argc[3])==0){
			n = std::stoi(argc[2]);
			t =  std::stoi(argc[4]);
		}
		else if(strcmp("-n", argc[3])==0 && strcmp("-t", argc[1])==0){
			n = std::stoi(argc[4]);
			t =  std::stoi(argc[2]);
		}
		else {std::cerr<<"Pls provide arguments"<<std::endl; return 0;}
	}
	
	std::thread tList[t];
	for(int i=0; i<t;i++){
		tList[i] = std::thread(threadFunc, n);
	}
	for(int i=0; i<t; i++) tList[i].join();

	return 0;
} // main
