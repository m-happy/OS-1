#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;
ofstream rep1;
ifstream in;
class Process{
    
    public:
        void set_attr(int pid,int t,int p,int k,int i){
            process_id = pid; 
            this->t=t;
            this->period = p;
            this->k = k;
            this->deadline = p;
            this->time_left = t;
            this->index = i;
            this->q_s = false;
        }
        int process_id;
        int t; //processing_time
        int period; //deadline
        int k; //number of time process repeats
        bool q_s; //status in queue
        int deadline; //deadline
        int index; 
        int time_left;
        bool on_cpu; //whether it is executing on CPU or not
};

void deadline_gone(int t,Process j){ // A function to print Deadline miss
    rep1<<"Deadline miss by Process"<<j.process_id<<" at time "<<t<<endl;
}

struct less_than_key //Sorting function comparator
{
    inline bool operator() (Process* struct1, Process* struct2)
    {
        return (struct1->period < struct2->period);
    }
};



int main(){
    int n;
    in.open("inp.txt"); // Reads from inp.txt
    in>>n;
    vector<Process> jobs(n);
    for(int i=0;i<n;i++){
        int pid,t,p,k;
        in>>pid>>t>>p>>k;
        jobs[i].set_attr(pid,t,p,k,i);
    }
    in.close();
    rep1.open("RM-Log.txt");

    vector<Process*> rq;
    Process running;
    running.on_cpu = false;
    int t=0;
    int fail=0;
    int total=0;
    double turnaround_time=0;
    for(int i=0;i<n;i++) total+=jobs[i].k;

    for(int i=0;i<n;i++){
        rep1<<"Process P"<<jobs[i].process_id<<": processing time="<<jobs[i].t<<"; deadline:"
        <<jobs[i].deadline<<"; period:"<<jobs[i].period<<" joined the system at time 0\n";
    }
    double avg_wait_time =0;
    while(1){
        bool flag = true;
        for(int i=0;i<n;i++){
            if(jobs[i].k>0) flag=false;
        }
        if(flag==true) break;
        //Flags the running process as false
        if(running.on_cpu ==true && t%running.period==0) {
            running.on_cpu = false;
            if(jobs[running.index].k>0){
                int x = running.index;
                jobs[x].q_s = true;
                rq.push_back(&jobs[x]);
            }
        }
        //Adds the processes whose period is here and removes the processes whose deadline is expired
        for(int i=0;i<n;i++){
            if(t%jobs[i].period==0 && jobs[i].k>0){
                //Process at its period is pushed in the queue
                if(jobs[i].q_s==false){
                    jobs[i].q_s=true;
                    rq.push_back(&jobs[i]);
                }
                else{
                    //Deadline expired 
                    turnaround_time+=jobs[i].period; 
                    avg_wait_time+= t -  jobs[i].deadline+jobs[i].period - (jobs[i].t-jobs[i].time_left)+1;

                    deadline_gone(t,jobs[i]);
                    fail++;
                    jobs[i].time_left = jobs[i].t;
                    jobs[i].deadline+=jobs[i].period;
                    jobs[i].k--;
                }
            }
        }
        sort(rq.begin(),rq.end(),less_than_key()); //sorts the queue according to period
        
        if(running.on_cpu==false){
            if(!rq.empty()){
                //is no process executing , we select minimum period process from ready queue
                running = *rq[0];
                if(jobs[running.index].time_left == running.t){
                    rep1<<"Process P"<<running.process_id<<" starts execution at time "<<t<<".\n";
                }
                else{
                    rep1<<"Process P"<<running.process_id<<" resumes execution at time "<<t<<".\n";
                }
                rq.erase(rq.begin()); 
                jobs[running.index].q_s =false;
                running.on_cpu = true;
            }
            else{
                //if ready queue is empty we run the loop till CPU is not idle or another processs causes period
                bool flag=true;
                while(flag==true){
                    t++;
                    int i=0;
                    while(i<n){
                        if(t%jobs[i].period==0 && jobs[i].k>0) {flag=false; break;}
                        i++;
                    }
                }
                rep1<<"CPU is idle till time "<<t-1<<".\n";
                continue;
            }
        }

        
        if(!rq.empty() && rq[0]->period < jobs[running.index].period) { //preemption
            // if the top of queue has period less than running process , it is preempted and runnign process is pushed to  queue
            Process r = *rq[0];
            jobs[r.index].q_s = false;
            rq.erase(rq.begin());
            rq.push_back(&jobs[running.index]);
            rep1<<"Process P"<<running.process_id<<" is preempted by Process P"<<r.process_id
            <<" at time "<<t<<". Remaining processing time:"<<jobs[running.index].time_left<<"\n";
            rep1<<"Process P"<<r.process_id<<" starts execution at time "<<t<<endl;
            jobs[running.index].q_s =true;
            running = r;
            running.on_cpu=true;
            //cout<<t<<"-"<<t+1<<" "<<jobs[running.index].process_id<<" "<<jobs[running.index].time_left<<endl; //outputing status of execution at every point
            jobs[running.index].time_left--; //reduce current running time
            t++;
            continue;
        }
            //cout<<t<<"-"<<t+1<<" "<<jobs[running.index].process_id<<" "<<jobs[running.index].time_left<<endl; //outputing status of execution at every point

        jobs[running.index].time_left--; //reduce current running time
        if(jobs[running.index].time_left<=0) { // removes the processes which ended
                rep1<<"Process P"<<running.process_id<<" finishes execution at time "<<t<<".\n";
                turnaround_time+=t-jobs[running.index].deadline+jobs[running.index].period;
                //Total time between process to finish and its arrival
                avg_wait_time+= t -  jobs[running.index].deadline+jobs[running.index].period - jobs[running.index].t+1;
                //time spend by process in ready queue with no CPU
                jobs[running.index].time_left = running.t;           
                jobs[running.index].k--;
                jobs[running.index].q_s=false;
                running.on_cpu = false;
                jobs[running.index].deadline += jobs[running.index].period;
        }
        t++;
    }
    //cout<<avg_wait_time<<endl;
    avg_wait_time = avg_wait_time/total;
    turnaround_time = turnaround_time/total;
    ofstream av;
    av.open("Average_times.txt");
    av<<"RMS : Average wait time : "<<avg_wait_time<<"; Average turnaround time : "<<turnaround_time<<endl;
    av.close();
    rep1.close();
    rep1.open("RM-Stats.txt");
    rep1<<"Number of Processes entering the system : "<<total<<endl;
    rep1<<"Number of Processes successfully completed : "<<total-fail<<endl;
    rep1<<"Numer of Processes which missed the deadline : "<<fail<<endl;
    rep1.close();
    

}