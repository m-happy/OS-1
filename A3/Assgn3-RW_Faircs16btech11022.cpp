/*
OS2 : CS3523
Programming Assignment 3 
Readers Writers Solution
Mayank Hooda
*/

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;
#define ll  long long
ll nw,nr,csSeed,remSeed;
ll kw[10000],kr[10000];
ll read_count = 0;
sem_t mutex,rw,in;
FILE *f = fopen("FairRW-log.txt","w");
double wait_time;
default_random_engine eng1,eng2;
uniform_int_distribution<int> cs(1000,1000000);
uniform_int_distribution<int> rem(1000,1000000);
string getTime(time_t input)
{
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}

void* reader(void *ptr){
    ll id = (ll)ptr;
    ll count= 0;
    while(1){
        if(kr[id]==0) pthread_exit(0);
        count++;

        time_t now1=time(NULL);
        fprintf(f,"%lldst CS request by Reader Thread %lld at %s\n",count,id,(getTime(now1).c_str()));

        sem_wait(&in);
        sem_wait(&mutex);
        read_count++;
        if(read_count==1) sem_wait(&rw);
        sem_post(&mutex);
        sem_post(&in);

        time_t now2=time(NULL);
        wait_time+=now2-now1;
        fprintf(f,"%lldst CS entry by Reader Thread %lld at %s\n",count,id,(getTime(now1).c_str()));
        eng1.seed(csSeed);
        usleep(cs(eng1));
        fprintf(f,"%lldst CS exit by Reader Thread %lld at %s\n",count,id,(getTime(now1).c_str()));

        sem_wait(&mutex);
        read_count--;
        if( read_count ==0) sem_post(&rw);
        sem_post(&mutex);

        time_t now3 = time(NULL);
        eng2.seed(remSeed);
        usleep(rem(eng2));


        
        
        kr[id]--;
    }



}


void* writer(void *ptr){
    ll id = (ll)ptr;
    ll count= 0;
    
    while(1){
        if(kw[id]==0) pthread_exit(0);
        count++;
        time_t now1=time(NULL);
        struct tm * t1;
        fprintf(f,"%lldst CS request by Writer Thread %lld at %s\n",count,id,(getTime(now1).c_str()));
        sem_wait(&in);
        sem_wait(&rw);
        time_t now2=time(NULL);
        wait_time+=now2-now1;
        fprintf(f,"%lldst CS entry by Writer Thread %lld at %s\n",count,id,(getTime(now1).c_str()));
        eng1.seed(csSeed);
        usleep(cs(eng1));
        fprintf(f,"%lldst CS exit by Writer Thread %lld at %s\n",count,id,(getTime(now1).c_str()));

        sem_post(&rw);
        sem_post(&in);

        time_t now3 = time(NULL);
        eng2.seed(remSeed);
        usleep(rem(eng2));

        kw[id]--;
    }



}







int main(int argc, char *argv[]){
    ifstream fin("inp-params.txt");
    fin>>nw>>nr;
    ll x,y;
    fin>>x>>y;
    fin>>csSeed>>remSeed;
    for(int i=0;i<nw;i++) kw[i] = x;
    for(int i=0;i<nr;i++) kr[i] = y;
    /* n is number of threads while k is access frequency */
    pthread_t readers[nw];
    pthread_t writers[nr]; 
 
    /*Thread id's*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    sem_init(&rw,0,1);
    sem_init(&mutex,0,1);
    sem_init(&in,0,1);

    /* get the default attributes */
 
    for(ll i=0;i<max(nr,nw);i++){
        /*Create n workers*/
        if(i<nr) pthread_create(readers+i,&attr,reader,(void*)i);
       	if(i<nw) pthread_create(writers+i,&attr,writer,(void*)i);   
    }

    for(ll i=0;i<nr;i++){
        pthread_join(readers[i],NULL);
    }
    for(ll i=0;i<nw;i++){
	pthread_join(writers[i],NULL);
    }
    /*Join the threads*/
    fclose(f);
    f = fopen("Average_Times.txt","a");
    fprintf(f,"Average wait time of Fair_Reader_Writer is %lf",wait_time/(nw*x+nr*y));
    fclose(f);

}
