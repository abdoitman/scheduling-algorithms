#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <queue>
#include <math.h>

using namespace std;

int cycle = 0;  //~defining the current cycle we're on
int finished_processes = 0;
string output_filename;    //~defining the filename we're outputing to
int runtime;    //~defining how much time the CPU has been working
string strings[4];  //~used to split the input string

void write_to_file(string str){
    fstream outfile;
    outfile.open(output_filename , std::ios_base::app | std::ios_base::in);
    if (outfile.is_open())
    {
        outfile << str;
        outfile.close();
    }
}

class process
{
private:
    string process_ID;
    int arrival_time;
    int CPU_time;
    int IO_time;
    int remaining_CPU_time;
    int remaining_IO_time;
    bool IO_flag;

    string process_status;
    int finish_time;
    int TAT;

public:

    bool erased_from_blocked = false;   //flag to determine if the process has benn deleted from the blocked vector

    int get_arrival_time(){
        return arrival_time;
    }

    string get_name(){
        return process_ID;
    }

    void modify_status(string status){
        process_status = process_ID + ":"  + status + " ";
        if(status == "Running"){
            remaining_CPU_time--;
            runtime++;
        }
        if(status == "Blocked") remaining_IO_time--;
        write_to_file(process_status);
    }

    bool is_IO_finished(){
        if(!remaining_IO_time){ 
            if(IO_flag) remaining_CPU_time = CPU_time; //if a process has an IO time, its CPU should reset after finishing IO time
            IO_flag = false;
            return true;
        } else return false;
    }

    bool is_CPU_finished(){
        if(!remaining_CPU_time) return true;
        else return false;
    }

    bool is_finished(int finish){
        if(is_IO_finished() && is_CPU_finished()){
            finish_time = finish;
            finished_processes++;
            TAT = finish_time - arrival_time;
            if(!CPU_time) TAT++;
            if(!CPU_time && !IO_time) TAT = 0;  //checking if it's a dummy process with no CPU nor IO
            return true;
        }
        else return false;
    }

    int get_TAT(){
        return TAT;
    }

    process(string name, int CPU, int IO, int AT);
    ~process();
};

process::process(string name, int CPU, int IO, int AT)
{
    process_ID = name;
    arrival_time = AT;
    CPU_time = CPU;
    IO_time = IO;

    TAT = 0;
    IO_flag = false;

    if(IO_time > 0) //set the IO flag, meaning CPU time will be excuted both before and after the IO time
    {
        remaining_CPU_time = CPU_time;
        remaining_IO_time = IO_time;
        IO_flag = true;
    }
    else {
        remaining_CPU_time = CPU_time;
        remaining_IO_time = 0; 
    }
}

process::~process()
{
}

bool sort_by_AT(process& p01, process& p02){
    if(p01.get_arrival_time() == p02.get_arrival_time()) return p01.get_name() < p02.get_name();
    else return p01.get_arrival_time() < p02.get_arrival_time();
}

bool sort_by_name(process& p01, process& p02){
    return p01.get_name() < p02.get_name();
}

int len(string str){
    int length = 0;
    for(int i =0; str[i] != '\0'; i++) length++;
    return length;
}

void split(string str, char delim){
    int CurrentIndex = 0, i =0;
    int StartIndex = 0, EndIndex = 0;
    while (i <= len(str)){
        if ((str[i] == delim) || i == len(str)){
            EndIndex = i;
            string subStr;
            subStr.append(str,StartIndex,EndIndex - StartIndex);
            strings[CurrentIndex] = subStr;
            CurrentIndex++;
            StartIndex = EndIndex + 1;
        }
        i++;        
    }   
}

void print_TAT(vector<process>& input){
    for (int i = 0; i < input.size(); i++)
    {
        write_to_file("Turnaround time of process " + input.at(i).get_name() + ": " + to_string(input.at(i).get_TAT()) + '\n');
    }
}

queue<process> ready;
vector<process> blocked;

int main(){

    string filename; //the input file
    cout << "Please enter test file name : ";
    cin >> filename;

    vector<process> Processes;  //Making a vector containing all the processes
    vector<process> executed_processes; 

    fstream newfile;
    newfile.open(filename, ios::in);
    if (newfile.is_open())
    {
        string tp;
        while(getline(newfile,tp)){
            split(tp,' ');
            process temp = process(strings[0] , stoi(strings[1]), stoi(strings[2]), stoi(strings[3]));
            Processes.push_back(temp);
        };
    };

    sort(Processes.begin() , Processes.end() ,&sort_by_AT); //sorting the vector according to the arriving time of each process

    bool working = false;   //~Keeps the algorithm running untill all processes finish 
    process *running_process = NULL;

    if(Processes.size() > 0){ 
        working = true;
        cycle = Processes[0].get_arrival_time(); 
    } else {
        cerr << "Input file is empty or not valid!";
        return 0;
    }

    int counter = 0; //counting to tell if reached the time quantum

    char scheduling_type;   // 1 -> FCFS algorithm. 2 -> Round Robin algorithm
    cout << "Please enter the desire scheduling algorithm (1 for FCFS, 2 for Round Robin) : ";
    cin >> scheduling_type;

    if(scheduling_type == '1'){ //FCFS

        output_filename = "OutputFCFS.txt";

        std::ofstream ofs;  //clearing the output text file before writing again
        ofs.open("OutputFCFS.txt", std::ofstream::out | std::ofstream::trunc);
        ofs.close();

        while(working){ // while there are processes in the vector
        write_to_file(to_string(cycle) + " : ");

        // This loop is to push ready processes in the ready queue
        for(int i = 0; i < Processes.size(); i++)
        {
            if(Processes[i].get_arrival_time() == cycle) // when the arrival time of the ith process matches the current cycle we're on
            {
                // except it has no cpu time and it is supposed to be in blocked state
                if(Processes[i].is_CPU_finished() && !Processes[i].is_IO_finished())
                {
                    blocked.push_back(Processes[i]);
                    continue;
                }
                else if(Processes[i].is_finished(cycle)){ //checking if a process has no CPU nor IO time
                    continue;
                }
                //else, the process should be ready
                ready.push(Processes[i]);
                
                if(running_process != NULL || ready.size() != 1){ //Making sure the process can't be ready & running at the same time
                    Processes[i].modify_status("Ready");
                }
            }
        }

        // we're not at the beginning & there was a running process, we want to check if it is finished or not
        if(cycle != Processes[0].get_arrival_time() && running_process != NULL)
        {
            if(running_process->is_CPU_finished()) // finished its CPU time
            {
                if(!running_process->is_IO_finished()) // the process hasn't finish the IO yet
                {
                    blocked.push_back(*running_process); // process is blocked
                }
                else // the process finished its IO
                {
                    running_process->is_finished(cycle); // current process is finished
                    executed_processes.push_back(*running_process);
                }
                running_process = NULL;
            }
        }

        if(running_process == NULL) //There isn't any running processes in the CPU
        {
            if(!ready.empty()) // if the ready queue is not empty
            {
                running_process = &ready.front(); // pick another process from the ready queue
                ready.pop();
            }
        }

        // The ready queue is not empty and there's still a running process
        if(running_process != NULL) running_process->modify_status("Running");

        // Check if there are any other processes left
        if(finished_processes == Processes.size()){
            working = false;
            continue;
        }

        for (int i = 0; i < blocked.size(); i++)    //checking the blocked vector for any process(es) that has finished its I/O time
        {
            if(!blocked[i].erased_from_blocked) blocked[i].modify_status("Blocked"); //if the process isn't erased from vvector, make it blocked

            if(blocked[i].is_IO_finished() && !blocked[i].erased_from_blocked){
                if(blocked[i].is_CPU_finished()){ //if a process has 0 CPU then it shouldn't go back to the ready queue
                    blocked[i].is_finished(cycle);
                    executed_processes.push_back(blocked[i]);
                } else{
                    ready.push(blocked[i]);
                }
                blocked[i].erased_from_blocked = true;
            }
        }
        cycle++;
        write_to_file(" \n");
    }

    } else if(scheduling_type == '2'){

        int q;  //Quantum Time
        cout << "Please enter the time quantum : ";
        cin >> q;

        output_filename = "OutputRR.txt";

        std::ofstream ofs;  //clearing the output text file before writing again
        ofs.open("OutputRR.txt", std::ofstream::out | std::ofstream::trunc);
        ofs.close();

        while(working){
            write_to_file(to_string(cycle) + " : ");

            for (int i = 0; i < Processes.size() ; i++){    //checking for any new ready processes 
                    if(Processes[i].get_arrival_time() == cycle){   //if its arrival time = the cycle we're currently on
                        if(Processes[i].is_CPU_finished() && !Processes[i].is_IO_finished()){ //checking if process has 0 CPU time
                            blocked.push_back(Processes[i]);
                            continue;
                        } else if(Processes[i].is_finished(cycle)) continue; //checking if process has 0 CPU time and 0 IO time

                        ready.push(Processes[i]);
                        
                        if(running_process != NULL || ready.size() != 1) //this makes sure a process cannot be ready and running at the same time
                        {
                            Processes[i].modify_status("Ready"); 
                        }
                    }
                }
                
                if(cycle != Processes[0].get_arrival_time() && running_process != NULL){ //checking if a running process finished before the time quantum

                    if(running_process->is_CPU_finished()){

                        if(!running_process->is_IO_finished()){ //if it has finished its cpu time only and has IO time, it should be blocked
                            blocked.push_back(*running_process);
                        }
                        else {
                            running_process->is_finished(cycle); //current running process has finished its CPU and IO time, 
                            executed_processes.push_back(*running_process);
                        }
                        running_process = NULL; //the cpu has finished its running process, so running process is equal to NULL untill it picks up the next ready process from the queue
                        counter = q;
                    }
                }

                if(cycle == Processes[0].get_arrival_time() || counter == q){
                    //if the time quantum has finished but the process isn't finished yet, it'll be pushed again into the ready queue
                    if(cycle != Processes[0].get_arrival_time() && running_process != NULL) {
                        if(!running_process->is_CPU_finished()) ready.push(*running_process);
                    }  
                    if(!ready.empty()){
                    running_process = &ready.front();
                    ready.pop();
                    counter = 0;
                    }
                }
                if(running_process != NULL) running_process->modify_status("Running"); //making sure if there is a running process in the cpu

            if(finished_processes == Processes.size()){
                working = false;
                continue;
            }

            for (int i = 0; i < blocked.size(); i++)    //checking the blocked vector for any process(es) that has finished its I/O time
            {
                if(!blocked[i].erased_from_blocked) blocked[i].modify_status("Blocked"); //if the process isn't erased from vector, make it blocked

                if(blocked[i].is_IO_finished() && !blocked[i].erased_from_blocked){

                    if(blocked[i].is_CPU_finished()){ //if a process has 0 CPU then it shouldn't go back to the ready queue
                        blocked[i].is_finished(cycle);
                        executed_processes.push_back(blocked[i]);
                    } else{
                        ready.push(blocked[i]);
                    }
                    blocked[i].erased_from_blocked = true;  //mark the finished process as erased
                }
            }

            if(counter != q) counter++;
            cycle++;
            write_to_file(" \n");
        }

    } else {
        
        cerr << "Please enter a valid input (1 for FCFS & 2 for RR)";
        return 0;
    }

    float cpu_utilization = (float)runtime / (float)cycle;
    sort(executed_processes.begin() , executed_processes.end() ,&sort_by_name);
    
    write_to_file("\n\n");
    write_to_file("CPU Utilization = " + to_string(cpu_utilization) + '\n');
    write_to_file("Finish Time : " + to_string(cycle - 1) + '\n');
    print_TAT(executed_processes); 


    cout << "\nDone!\n\n";
    return 0;
}
