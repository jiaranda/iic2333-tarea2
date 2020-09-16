#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef BOOL_H
#define BOOL_H
#include <stdbool.h>
#endif

typedef enum status {
    NOT_ARRIVED,
    READY,
    RUNNING,
    WAITING,
    FINISHED,
    DEADLINE
} Status;

typedef struct process {
    pid_t pid;
    float priority;
    uint32_t arrival_time;
    uint32_t deadline;
    Status status;
    uint32_t burst_time[255];
    uint32_t waiting_time[255];
    uint32_t burst_time_len;
    uint32_t started_waiting_time;
    uint32_t current_burst; // valor de n actual
    int32_t status_times[6];

    //output_variables
    uint32_t initial_burst_total_time;
    char name[32];
    uint32_t CPU_count;
    uint32_t interrupt_count;
    uint32_t turnaround_time;
    uint32_t response_time;
    uint32_t waiting_total_time;
    bool process_finished_bf_deadline;
} Process;


Process* process_init(pid_t pid, 
                      char* name, 
                      uint32_t arrival_time, 
                      uint32_t deadline, 
                      uint32_t* burst_time, 
                      uint32_t* waiting_time,
                      uint32_t burst_time_len
                      );

void process_destroy(Process* process);

void process_print(Process* process);