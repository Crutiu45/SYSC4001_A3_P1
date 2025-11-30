/**
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @brief template main.cpp file for Assignment 3 Part 1 of SYSC4001
 * 
 */

#include<interrupts_EhimareIsoa_AshfaqulAlam.hpp>

void FCFS(std::vector<PCB> &ready_queue) {
    std::sort( 
                ready_queue.begin(),
                ready_queue.end(),
                []( const PCB &first, const PCB &second ){
                    return (first.arrival_time > second.arrival_time); 
                } 
            );
}

int find_index_by_pid(const std::vector<PCB> &v, int pid) {
    for (size_t i = 0; i < v.size(); ++i) if (v[i].PID == pid) return (int)i;
    return -1;
}

std::tuple<std::string /* add std::string for bonus mark */ > run_simulation(std::vector<PCB> list_processes) {

    std::vector<PCB> ready_queue;   //The ready queue of processes
    std::vector<PCB> wait_queue;    //The wait queue of processes
    std::vector<PCB> job_list;      //A list to keep track of all the processes. This is similar
                                    //to the "Process, Arrival time, Burst time" table that you
                                    //see in questions. You don't need to use it, I put it here
                                    //to make the code easier :).
    std::vector<unsigned int> wait_wakeup; 

    unsigned int current_time = 0;
    PCB running;

    //Initialize an empty running process
    idle_CPU(running);

    std::string execution_status;

    //make the output table (the header row)
    execution_status = print_exec_header();

    //Loop while till there are no ready or waiting processes.
    //This is the main reason I have job_list, you don't have to use it.
    while(!all_process_terminated(job_list) || job_list.empty()) {

        //Inside this loop, there are three things you must do:
        // 1) Populate the ready queue with processes as they arrive
        // 2) Manage the wait queue
        // 3) Schedule processes from the ready queue

        //Population of ready queue is given to you as an example.
        //Go through the list of proceeses
        for(auto &process : list_processes) {
            if(process.arrival_time == current_time) {//check if the AT = current time
                //if so, assign memory and put the process into the ready queue
                bool mem_ok = assign_memory(process);

                if(!mem_ok) {
                    process.state = NOT_ASSIGNED;
                    job_list.push_back(process);
                    execution_status += print_exec_status(current_time, process.PID, NEW, NOT_ASSIGNED);
                } else {
                process.state = READY;  //Set the process state to READY
                ready_queue.push_back(process); //Add the process to the ready queue
                job_list.push_back(process); //Add it to the list of processes

                execution_status += print_exec_status(current_time, process.PID, NEW, READY);
                }
            }
        }

        ///////////////////////MANAGE WAIT QUEUE/////////////////////////
        //This mainly involves keeping track of how long a process must remain in the ready queue
        if (!wait_queue.empty()) {
            std::vector<PCB> still_waiting;
            std::vector<unsigned int> still_wakeup;

            for (size_t i = 0; i < wait_queue.size(); ++i) {
                if (wait_wakeup[i] <= current_time) {

                    PCB proc = wait_queue[i];
                    proc.state = READY;
                    proc.cpu_since_last_io = 0;

                    ready_queue.push_back(proc);
                    execution_status += print_exec_status(current_time, proc.PID, WAITING, READY);
                    sync_queue(job_list, proc);

                } else {
                    still_waiting.push_back(wait_queue[i]);
                    still_wakeup.push_back(wait_wakeup[i]);
                }
            }

            wait_queue.swap(still_waiting);
            wait_wakeup.swap(still_wakeup);
        }
        /////////////////////////////////////////////////////////////////

        //////////////////////////SCHEDULER//////////////////////////////
        if (running.PID == -1) {
            if (!ready_queue.empty()) {
                // EP: lower PID == higher priority (same selection code as above)
                int best_idx = 0;
                for (size_t i = 1; i < ready_queue.size(); ++i)
                    if (ready_queue[i].PID < ready_queue[best_idx].PID)
                        best_idx = (int)i;

                if ((size_t)best_idx != ready_queue.size() - 1)
                    std::swap(ready_queue[best_idx], ready_queue.back());

                run_process(running, job_list, ready_queue, current_time);
                execution_status += print_exec_status(current_time, running.PID, READY, RUNNING);
            }
        } else {
            // 1. Decrement CPU time first
            if (running.remaining_time > 0)
                running.remaining_time--;

            // 2. Increment CPU time since last I/O
            running.cpu_since_last_io++;

            // 3. Check if process finishes BEFORE triggering I/O
            if (running.remaining_time == 0) {
                execution_status += print_exec_status(current_time, running.PID, RUNNING, TERMINATED);
                terminate_process(running, job_list);
                idle_CPU(running);
            }
            // 4. Trigger I/O EXACTLY when cpu_since_last_io == io_freq
            else if (running.io_freq > 0 && running.cpu_since_last_io == running.io_freq) {

                unsigned int wake_at = current_time + running.io_duration;

                execution_status += print_exec_status(current_time, running.PID, RUNNING, WAITING);

                running.state = WAITING;

                wait_queue.push_back(running);
                wait_wakeup.push_back(wake_at);

                sync_queue(job_list, running);

                idle_CPU(running);
            }
        }


        /////////////////////////////////////////////////////////////////

        if (running.PID == -1) {
            if (!ready_queue.empty()) {
                // EP: lower PID == higher priority
                int best_idx = 0;
                for (size_t i = 1; i < ready_queue.size(); ++i) {
                    if (ready_queue[i].PID < ready_queue[best_idx].PID)
                        best_idx = (int)i;
                }
                if ((size_t)best_idx != ready_queue.size() - 1)
                    std::swap(ready_queue[best_idx], ready_queue.back());

                run_process(running, job_list, ready_queue, current_time);
                execution_status += print_exec_status(current_time, running.PID, READY, RUNNING);
            }
        }

        current_time++;
    }
    
    //Close the output table
    execution_status += print_exec_footer();

    return std::make_tuple(execution_status);
}


int main(int argc, char** argv) {

    //Get the input file from the user
    if(argc != 2) {
        std::cout << "ERROR!\nExpected 1 argument, received " << argc - 1 << std::endl;
        std::cout << "To run the program, do: ./interrutps <your_input_file.txt>" << std::endl;
        return -1;
    }

    //Open the input file
    auto file_name = argv[1];
    std::ifstream input_file;
    input_file.open(file_name);

    //Ensure that the file actually opens
    if (!input_file.is_open()) {
        std::cerr << "Error: Unable to open file: " << file_name << std::endl;
        return -1;
    }

    //Parse the entire input file and populate a vector of PCBs.
    //To do so, the add_process() helper function is used (see include file).
    std::string line;
    std::vector<PCB> list_process;
    while(std::getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();

    //With the list of processes, run the simulation
    auto [exec] = run_simulation(list_process);

    write_output(exec, "execution.txt");

    return 0;
}