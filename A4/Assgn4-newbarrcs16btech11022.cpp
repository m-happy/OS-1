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

FILE* f=fopen("new-barr-log.txt","w");

sem_t mutec,barrier1,barrier2;

ll n,k,ki,preLam,postLam;

default_random_engine eng1,eng2; //Random number generation engines
uniform_int_distribution<int> preL(10000,90000);
uniform_int_distribution<int> postL(10000,90000);


double wait_time[10000]; //Calculates waiting time of each thread
int count_th; // counts the current thread number waiting on barrier


string getTime(time_t input){ //Get formatted string time
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}
void barrier_point(){
    sem_wait(&mutec);// for making count increment atomic
        count_th = count_th + 1;
        if (count_th == n) {
            sem_wait(&barrier2);
            sem_post(&barrier1);//last thread allows the first thread to move on
        }
    sem_post(&mutec);

        sem_wait(&barrier1);//all threads except last wait here on barrier
        sem_post(&barrier1);//each thread frees the next thread to move on

        /* Now the barrier1 is not in it's initial state so it has to be made to wait again */
    sem_wait(&mutec);//Atomicity
        count_th=count_th-1;//Decrements counter
        if(count_th==0) {
            sem_wait(&barrier1);//Barrier 1 in initial position
            sem_post(&barrier2);//Barrier 2 again serves the same purpose of making all the threads wait
        }
    sem_post(&mutec);
        sem_wait(&barrier2);//Waits for the last thread to free it
        sem_post(&barrier2);
}

void* testBarrier(void *ptr){
    ll id = (ll)ptr;
    int beforeBarrSleep , afterBarrSleep;
    time_t t1 = time(NULL);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (int barrIter =0;barrIter<k;barrIter++){//k iterations of the thread
        eng1.seed(preLam);
        fprintf(f,"Going to sleep before the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);
        usleep(preL(eng1));//sleeps before barrier invocation

        fprintf(f,"Before the %dth barrier invocation at time : %s by thread %lld\n",barrIter+1,getTime(time(NULL)).c_str(),id);
          
        barrier_point();

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
    /* n is number of threads while k is access frequency */
    pthread_t workers[n]; 
    /*Thread id's*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    sem_init(&mutec,0,1);
    /*for letting one thread pass*/
    sem_init(&barrier1,0,0);
    /*Blocks incoming threads at barrier */
    sem_init(&barrier2,0,1);




    /* get the default attributes */
 
    for(ll i=0;i<n;i++){
        /*Create n workers*/
        pthread_create(workers+i,&attr,testBarrier,(void*)i);
    }
    /*Join the threads*/
    for (int i = 0; i < n; i++) pthread_join(workers[i], NULL);
    fclose(f);
    f = fopen("Average_times.txt","w");
    double total =0;
    for(int i=0;i<n;i++){
        fprintf(f,"Average wait time by thread %d is %lf\n",i+1,wait_time[i]/1000);
        total+=wait_time[i];
    }
    fprintf(f,"Average wait time of new barr : %lf\n",total/(1000*n*k));

}
