/*
OS2 : CS3523
Programming Assignment 2 
Critical Section Solution
Mayank Hooda
*/

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <atomic>
//#include <sys/time.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
using namespace std;
#define ll  long long
#define ld  long double
typedef long double Random;
FILE* f=fopen("CAS-Log.txt","w");
double wait_time;
ll n,k[1000],ki,csSeed,remSeed;
default_random_engine eng1,eng2;
uniform_int_distribution<int> cs(100,200);
uniform_int_distribution<int> rem(100,200);

atomic<int>locki(0);

/*Checks whether all are threads are fullfilled*/
bool flag = true;

//get formatted time
string getTime(time_t input)
{
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}

void* testCS(void *ptr){
    ll id = (ll)ptr;
    /*Calculating current system time */
    //timeval tv;
     do {
        if(k[id]==0) pthread_exit(0); 
        time_t now1=time(NULL);
        fprintf(f,"%lldst CS request by Thread %lld at %s\n",ki-k[id]+1,id,(getTime(now1).c_str()));
        while(1){
            int x=0,y=1;
            if(locki.compare_exchange_strong(x,y)) break;
        }
        
        //Critical Section
        time_t now2=time(NULL);
        wait_time+=now2-now1;
        fprintf(f,"%lldst CS entry by Thread %lld at %s\n",ki-k[id]+1,id,(getTime(now2).c_str()));
        eng1.seed(csSeed);
        usleep(cs(eng1));

        k[id]--;
        time_t now3 = time(NULL);

        locki=0;
        
        //remainder section
        
        fprintf(f,"%lldst CS exit by Thread %lld at %s\n",ki-k[id],id,getTime(now3).c_str());
        eng2.seed(remSeed);
        usleep(rem(eng2));

    }while(true);
    pthread_exit(0);
    
}









int main(int argc, char *argv[]){
    ifstream in("inp.txt");
    in>>n>>ki;
    in>>csSeed>>remSeed;
    for(ll i=0;i<n;i++) k[i] = ki;
    /* n is number of threads while k is access frequency */
    pthread_t workers[n]; 
    /*Thread id's*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /* get the default attributes */
 
    for(ll i=0;i<n;i++){
        /*Create n workers*/
        pthread_create(workers+i,&attr,testCS,(void*)i);
    }
    /*Join the threads*/
    for (int i = 0; i < n; i++) pthread_join(workers[i], NULL);
    fclose(f);
    f = fopen("Average_times.txt","a");
    fprintf(f,"Average wait time of CAS :%lf\n",wait_time*1000/(n*ki));

}