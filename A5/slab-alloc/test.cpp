#include <iostream>
#include <mutex>

std::mutex a[4];
using namespace std;
int main(){
	a[1].lock();
	a[1].unlock();	
	return 0;
}