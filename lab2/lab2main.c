//+
// File: lab2main.c
//
// Purpose: The main function and the initial functions
//          to be completed as part of ELEC 277 Lab 2.
//
// Copyright 2022 Iffy Maduabuchi, Thomas Dean
//

#include "common.h"


struct monitor_thread_info monitor_threads[MAX_MACHINES];
struct printer_thread_param printer_param;
struct reader_thread_param reader_param;
char * progname;

//---------------------------------------------------
// Semaphores
//---------------------------------------------------

const char * ACCESS_STATS_NAME = "/access_stats";
const char * ACCESS_SUMMARY_NAME = "/access_summary";
const char * MUTEX_NAME = "/mutex"; 

sem_t * access_stats;
sem_t * access_summary;
sem_t * mutex;

void init_shared_mem_seg( struct shared_segment * shmemptr );
void * reader_thread(void * parms);
void * printer_thread(void * parms);


// shared memory object
struct shared_segment shared_memory;

int main(int argc, char * argv[]){
    
    // save executable name in global for error messages;
    progname = argv[0];

    // seed the random generator
    srand((unsigned long)time(NULL));

     //check paramters
    if (argc != 3){
        fprintf(stderr,"usage: %s num_monitors printer_delay_millisecs\n", argv[0]);
        exit(1);
    }

    // First argument is number of machine monitors
    int num_monitor_threads = atoi(argv[1]);
    if (num_monitor_threads == 0){
        fprintf(stderr,"Could not convert <%s> to a number\n", argv[1]);
        exit(1);
    }

    testLog('M',"Monitor threads %d", num_monitor_threads);

    if (num_monitor_threads > MAX_MACHINES){
        fprintf(stderr,"Max number of montiros is %d\n", MAX_MACHINES);
        exit(1);
    }

    // Second argument is number of machine monitors
    int printer_delay = atoi(argv[2]);
    if (printer_delay == 0){
        fprintf(stderr,"Could not convert <%s> to a number\n", argv[2]);
        exit(1);
    }

    // delete the sempahores if they exist
    sem_unlink(ACCESS_STATS_NAME);
    sem_unlink(ACCESS_SUMMARY_NAME);
    sem_unlink(MUTEX_NAME);
    for (int i = 0; i < num_monitor_threads; i++){
        shared_memory.machine_stats[i].read=1;
    }

    init_shared(&shared_memory);

    testLog('M',"Printer delay (msec) %d",printer_delay);

    // default atrributes for initializing threads
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    // ---------------------------------------------------
    // START MONITOR TREADS
    
 
    // start a thread for each monitor.
    // the monitor_threads array is used to pass
    // parameters
    // ---------------------------------------------------
    for (int i=0; i < num_monitor_threads; i++) {
        monitor_threads[i].machine_number = i;
        monitor_threads[i].shmemptr = &shared_memory;
	// threaded version
        pthread_create(&(monitor_threads[i].thread_id), &thread_attr, monitor_thread_func, (void *)&(monitor_threads[i]));
    }
    
    // TODO: stage 2
    pthread_t reader_id;
    
    // start reader thread
    pthread_create(&reader_id, &thread_attr, reader_thread, (void *) &reader_param);

    // TODO: stage 3
    // start printer thread
    
    // ---------------------------------------------------
    // FINISH - use pthread_exit instead of return.
    // When the main program finishes all threads are 
    // terminated. Calling pthread_exit exits the main
    // thread while leaving other threads intact. When
    // all of the threads complete, the program will exit.
    // ---------------------------------------------------
    pthread_exit(0);
}

//******************************************************************************
//******************************************************************************
// Functions for you to write to complete the lab
//******************************************************************************
//******************************************************************************

//+
// Function: init_shared
// This function allocates and initializes the three sempaphores as well
// as initiallizing the shared object to the starting state
//-

void init_shared( struct shared_segment * shmemptr ){
    // Initialize Semaphores
    mutex = sem_open("/mutex",O_RDWR|O_CREAT,0660,1);
    access_summary = sem_open("/access_summary",O_RDWR|O_CREAT,0660,1);
    access_stats = sem_open("/access_stats",O_RDWR|O_CREAT,0660,1);

    // Check if semaphore initialization failed
    if (access_stats == SEM_FAILED || access_summary == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Semaphore failed to initialize. Exiting program.");
        exit(1);
    }
    
    // Initialize the members of the shared memory segment
	shmemptr -> monitorCount = 0;
}

//+
// Function monitor_update_status_entry
// This functon is called by the monitor thread for each entry in the monitor status
// files. There is a delay before each call based on the last field of each line.
// it updates the entry in shared memory with the data sent from the simulated remote machine.
// reader protocol of reader-writer problem.
//
// Parameters:
//    machine_id - the id of the machine that is updated
//    status_id - the number of the update (counts from 0) used only for information
//    cur_read_stat - pointer to data structure from simulated remote machine
//    shmemptr -  pointer to shared object.
//-

void monitor_update_status_entry(int machine_id, int status_id, struct status * cur_read_stat, struct shared_segment * shmemptr ){

    colourMsg(machId[machine_id] ,CONSOLE_GREEN,"Machine %d Line %d: %d,%d,%f,%d,%d",machine_id,status_id,
			     (cur_read_stat->machine_state),
			     (cur_read_stat->num_of_processes),
			     (cur_read_stat->load_factor),
			     (cur_read_stat->packets_per_second),
			     (cur_read_stat->discards_per_second));

    //------------------------------------
    //  enter critical section for monitor
    //------------------------------------
    sem_wait(mutex);
    shmemptr -> monitorCount++;
    if (shmemptr -> monitorCount == 1) sem_wait(access_stats);
    sem_post(mutex);
    //------------------------------------
    // monitor critical section
    //------------------------------------
    
 
    // store the monitor data
    shmemptr-> machine_stats[machine_id].machine_state = cur_read_stat->machine_state;
    shmemptr-> machine_stats[machine_id].num_of_processes= cur_read_stat->num_of_processes;
    shmemptr-> machine_stats[machine_id].load_factor = cur_read_stat->load_factor;
    shmemptr-> machine_stats[machine_id].packets_per_second = cur_read_stat->packets_per_second;
    shmemptr-> machine_stats[machine_id].discards_per_second = cur_read_stat->discards_per_second;
    shmemptr-> machine_stats[machine_id].timestamp = cur_read_stat->timestamp;

    // mark as unread
    shmemptr -> machine_stats[machine_id].read = 0;

    // report if overwritten or normal case (Stage 2)
    
    
    // if the read functopm has read the data before updating the data report it
    if (shmemptr -> machine_stats[machine_id].read = 1) {
        colourMsg(machId[machine_id], CONSOLE_RED, "Machine %d Line %d: %d,%d,%f,%d,%d [DATA OVERWRITTEN]",machine_id,status_id,
			     (cur_read_stat->machine_state),
			     (cur_read_stat->num_of_processes),
			     (cur_read_stat->load_factor),
			     (cur_read_stat->packets_per_second),
			     (cur_read_stat->discards_per_second));
    }

    else {
        colourMsg(machId[machine_id] ,CONSOLE_GREEN,"Machine %d Line %d: %d,%d,%f,%d,%d [DATA STORED]",machine_id,status_id,
			     (cur_read_stat->machine_state),
			     (cur_read_stat->num_of_processes),
			     (cur_read_stat->load_factor),
			     (cur_read_stat->packets_per_second),
			     (cur_read_stat->discards_per_second));
    }

    

    //------------------------------------
    // exit critical setion for monitor
    //------------------------------------
    sem_wait(mutex);
    shmemptr -> monitorCount--;
    if (shmemptr -> monitorCount == 0) sem_post(access_stats);
    sem_post(mutex);
}


// stage 2
/*
 Function: reader_thread
 This thread
        read and calculate the summary for new entries
        takes the pointer to the shared memory segment
        the pointer to the total entries read up to now
        and a pointer to max entries to read
        function should recalculate and update summary information for every new entry
        raise alarms for systems that are down
        update the total entries read
        if total_entries_read no longer less than max entries, should set no more information flag in summary information
*/


void * reader_thread(void * parms){
    struct shared_segment * shmemptr = ((struct reader_thread_param*)parms)->shmemptr;
    int num_machines = ((struct reader_thread_param*)parms)->num_machines;
    int more_updates = 1;
    unsigned read_update_times[MAX_MACHINES];
    int read_machines_state[MAX_MACHINES];
    
    int total_procs, total_pps, total_dps = 0;
    float total_lf = 0;
    
    long summary_checksum;
    
    threadLog('R',"Readeer Thread: %d machines", num_machines);
    msleep(1000);
    
    while(more_updates){
        threadLog('R',"Readeer Thread loop start", num_machines);

        // aquire stats semaphore
        sem_wait(access_stats);

        threadLog('R',"Readeer Thread loop accessing_stats lock aquired", num_machines);

        for (int machine_id = 0; machine_id < num_machines; machine_id++){
            // check for updates toeach machine
            if (shmemptr-> machine_stats[machine_id].read == 0) {
                // set read flag to true
                shmemptr-> machine_stats[machine_id].read == 1;
                // collect stats for all machines
                read_machines_state[machine_id] = shmemptr-> machine_stats[machine_id].machine_state;
                read_update_times[machine_id] = shmemptr-> machine_stats[machine_id].timestamp;

                // Print a warning if the machine is down
                if (shmemptr->machine_stats[machine_id].machine_state == 0) {
                    colourMsg(machine_id ,CONSOLE_RED,"Warning machine_id:%d is down", machine_id);
                }

                // Accumulate data of the machine
                total_procs += shmemptr-> machine_stats[machine_id].num_of_processes;
                total_pps += shmemptr-> machine_stats[machine_id].packets_per_second;
                total_dps += shmemptr-> machine_stats[machine_id].discards_per_second;
                total_lf += shmemptr-> machine_stats[machine_id].load_factor;
            }
        } 
        
        // release stats semaphore
        sem_post(access_stats);
        threadLog('R',"Readeer Thread loop  accessing_stats lock released", num_machines);


        //checksum - consume time outside of critical section.
        shmemptr->checksum_seed = gen_checksum_seed();
        summary_checksum = gen_summary_checksum();

        
        //=======================
        // do summary information (state 3)
        //=======================
        
        // lock summary semaphore
        
        // write summary checksum
        
        // update machine uptime sand last heard
        
        // calculate new averages
        
        // releast summary semaphore
        
        //=======================
        // are the monitors still running? (Stage 2)
        //=======================
        
        // Check if there's more updates, stop if theres no more
        if (shmemptr -> numMonitors == 0) more_updates = 0;

        threadLog('R',"Readeer Thread loop end", num_machines);
    }
    
    pthread_exit(0);
    // not reached.
    return NULL;
}

// stage 3

void * printer_thread(void * parms){
    struct shared_segment * shmemptr = ((struct printer_thread_param*)parms)->shmemptr;
    int print_period = ((struct printer_thread_param*)parms)->print_period;
    int num_machines = ((struct printer_thread_param*)parms)-> num_machines;
    int more_updates = 1;
    unsigned cur_uptime;
    unsigned cur_time;


    
    threadLog('P',"Printer Thread: delay %d", print_period);
    
    while(more_updates){
        
        // sleep for print_period
        msleep(print_period);
        
        // aquire summary mutex
        
        // get current time
        
        // printe summary
        threadLog('P',"Printer Step");

        printf("[%u] SUMMARY INFORMATION\n", get_current_unix_time());
        printf("MACHINE | UP | UPTIME                 | LAST UPDATE  \n");
        printf("-----------------------------------------------------\n");
        
        for (int i = 0; i < num_machines; i++){
        }
        
        // release summary mutex


        //Are the monitors still running.
    }
    
    pthread_exit(0);
    // not reached.
    return NULL;
}
