# scheduling-algorithms
Implemented both of First-Come-First-Served and Round Robin scheduling algorithms using OOP. The main idea is implementing different methods of how a uni-processor can handle several processes with various required CPU time, required I/O time and .


INPUT FORM:
-----------
The input should be a text file in the same directory as the code file and formated as follows:
PROCESS_ID CPUT_TIME I/O_TIME ARRIVAL_TIME

After running the program, it'll ask you to enter the input file name for example: "testcase.txt". If the name doesn't match any txt files in the directory the program will send an error and simply terminates.
Then it'll let the user choose what kind of scheduling algorithm they're willing to run:
1 -> First-Come-First-Served 
2 -> Round Robin
If the user entered '2', they'll be asked to enter the time quatum of the RR algorithm (q >= 1)


OUTPUT FORM:
------------
For both of the algorithms the output will be in a txt file form.
For the FCFS algorithm, the output file will be named "OutputFCFS.txt".
In case of RR algorithm, the output file will be named "OutputRR.txt".


NOTE:
-----
Don't worry about deletiing the output file after each run, the program wipes out its content at the beginning of each run.
