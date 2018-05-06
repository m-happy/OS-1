#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;
ofstream out;
int main(){
    out.open("inp.txt");
    int n;
    n=45;
    out<<n<<endl;
    srand(time(NULL));
    int arr[n*3];
    for(int i=0;i<n*3;i++) arr[i]=rand();
    for(int i=0;i<n*3;i+=3){
        out<<i/3+1<<" "<<10 + arr[i]%20<<" "<<60 + arr[i+1]%50<<" "<<5+arr[i+2]%5<<endl;
    }
}