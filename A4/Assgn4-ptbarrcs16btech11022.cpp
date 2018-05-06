/*OS2 : CS3523*/
/*Programming Assignment 4*/
/*Barrier Implementation using semaphore*/
/*Mayank Hooda*/

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
using namespace std;
#define ll  long long
#define ld  long double

FILE* f=fopen("pthread-barr-log.txt","w");

sem_t mutec,barrier1,barrier2;

ll n,k,ki,preLam,postLam;

default_random_engine eng1,eng2;
uniform_int_distribution<int> preL(1000,2000);
uniform_int_distribution<int> postL(1000,2000);


double wait_time[10000];
int count_th;
static pthread_barrier_t barrier;


/*Checks whether all are threads are fullfilled*/

//get formatted time
string getTime(time_t input){
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}

void* testBarrier(void *ptr){
    int s;
    ll id = (ll)ptr;
    int beforeBarrSleep , afterBarrSleep;
    time_t t1 = time(NULL);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int barrIter =0;barrIter<k;barrIter++){
        eng1.seed(preLam);
        fprintf(f,"Going to sleep before the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);
        usleep(preL(eng1));


        //Barrier implementation

        fprintf(f,"Before the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);
        //Barrier
        
        
        s = pthread_barrier_wait(&barrier);



        fprintf(f,"After the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);

        eng2.seed(postLam);
        fprintf(f,"Going to sleep after the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);
        usleep(postL(eng2));
    }
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    wait_time[id]=std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    pthread_exit(0);
}









int main(){
    ifstream in("inp-params.txt");
    in>>n>>k;
    int s;
    /* n is number of threads while k is access frequency */
    pthread_t workers[n]; 
    /*Thread id's*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    sem_init(&mutec,0,1);
    sem_init(&barrier1,0,0);
    sem_init(&barrier2,0,1);
    s = pthread_barrier_init(&barrier, NULL,n);






    /* get the default attributes */
 
    for(ll i=0;i<n;i++){
        /*Create n workers*/
        pthread_create(workers+i,&attr,testBarrier,(void*)i);
    }
    /*Join the threads*/
    for (int i = 0; i < n; i++) pthread_join(workers[i], NULL);
    fclose(f);
    f = fopen("Average_times.txt","a");
    double total =0;
    for(int i=0;i<n;i++){
        fprintf(f,"Average wait time by thread %d is %lf\n",i+1,wait_time[i]/1000);
        total+=wait_time[i];
    }

    fprintf(f,"Average wait time of pthread barr : %lf\n",total/(n*k*1000));

}
